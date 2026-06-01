/* gOS Kernel — E1000 Transmit Path
 * Reference: https://wiki.osdev.org/Intel_8254x
 */

#include "drivers/e1000/e1000.h"
#include "drivers/nic/nic.h"
#include "mm/pmm/pmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

#define E1000_TX_DESC_COUNT     8
#define E1000_TX_BUFFER_SIZE    2048

static e1000_tx_desc_t g_e1000_tx_ring[E1000_TX_DESC_COUNT];
static uint8_t         g_e1000_tx_buffers[E1000_TX_DESC_COUNT][E1000_TX_BUFFER_SIZE];
static uint32_t __attribute__((used)) g_e1000_tx_head = 0;
static uint32_t __attribute__((used)) g_e1000_tx_tail = 0;

static int __attribute__((used)) e1000_tx_init(uint64_t base) {
    memset(g_e1000_tx_ring, 0, sizeof(g_e1000_tx_ring));
    memset(g_e1000_tx_buffers, 0, sizeof(g_e1000_tx_buffers));

    for (int i = 0; i < E1000_TX_DESC_COUNT; i++) {
        g_e1000_tx_ring[i].addr = (uint64_t)g_e1000_tx_buffers[i];
        g_e1000_tx_ring[i].status = 1; /* Done */
    }

    uint64_t ring_phys = (uint64_t)(uintptr_t)g_e1000_tx_ring; /* In higher half */
    e1000_write_reg(base, E1000_TDBAL, (uint32_t)(ring_phys & 0xFFFFFFFF));
    e1000_write_reg(base, E1000_TDBAH, (uint32_t)(ring_phys >> 32));
    e1000_write_reg(base, E1000_TDLEN, E1000_TX_DESC_COUNT * sizeof(e1000_tx_desc_t));
    e1000_write_reg(base, E1000_TDH, 0);
    e1000_write_reg(base, E1000_TDT, 0);

    /* Enable transmitter */
    uint32_t tctl = e1000_read_reg(base, E1000_TCTL);
    tctl |= TCTL_EN | TCTL_PSP;
    tctl |= (0x10 << TCTL_CT_SHIFT); /* Collision threshold */
    tctl |= (0x40 << TCTL_COLD_SHIFT); /* Full-duplex */
    e1000_write_reg(base, E1000_TCTL, tctl);

    klog_info("E1000: TX initialized with %d descriptors", E1000_TX_DESC_COUNT);
    return 0;
}

int e1000_tx_packet(uint64_t base, const void *data, size_t len) {
    if (len > E1000_TX_BUFFER_SIZE) return -1;

    uint32_t tail = e1000_read_reg(base, E1000_TDT);
    uint32_t head = e1000_read_reg(base, E1000_TDH);

    if (((tail + 1) % E1000_TX_DESC_COUNT) == head) {
        klog_warn("E1000: TX ring full");
        return -1;
    }

    memcpy(g_e1000_tx_buffers[tail], data, len);
    g_e1000_tx_ring[tail].length = (uint16_t)len;
    g_e1000_tx_ring[tail].cmd = 0x01 | 0x08; /* EOP, RS */
    g_e1000_tx_ring[tail].status = 0;

    e1000_write_reg(base, E1000_TDT, (tail + 1) % E1000_TX_DESC_COUNT);

    /* Wait for transmit */
    for (int i = 0; i < 10000; i++) {
        if (g_e1000_tx_ring[tail].status & 0x01) break;
        io_wait();
    }

    return (int)len;
}

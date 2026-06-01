/* gOS Kernel — E1000 Receive Path
 * Reference: https://wiki.osdev.org/Intel_8254x
 */

#include "drivers/e1000/e1000.h"
#include "mm/pmm/pmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

#define E1000_RX_DESC_COUNT     8
#define E1000_RX_BUFFER_SIZE    2048

static e1000_rx_desc_t g_e1000_rx_ring[E1000_RX_DESC_COUNT];
static uint8_t         g_e1000_rx_buffers[E1000_RX_DESC_COUNT][E1000_RX_BUFFER_SIZE];

static int __attribute__((used)) e1000_rx_init(uint64_t base) {
    memset(g_e1000_rx_ring, 0, sizeof(g_e1000_rx_ring));
    memset(g_e1000_rx_buffers, 0, sizeof(g_e1000_rx_buffers));

    for (int i = 0; i < E1000_RX_DESC_COUNT; i++) {
        g_e1000_rx_ring[i].addr = (uint64_t)(uintptr_t)g_e1000_rx_buffers[i];
        g_e1000_rx_ring[i].status = 0;
    }

    uint64_t ring_phys = (uint64_t)(uintptr_t)g_e1000_rx_ring;
    e1000_write_reg(base, E1000_RDBAL, (uint32_t)(ring_phys & 0xFFFFFFFF));
    e1000_write_reg(base, E1000_RDBAH, (uint32_t)(ring_phys >> 32));
    e1000_write_reg(base, E1000_RDLEN, E1000_RX_DESC_COUNT * sizeof(e1000_rx_desc_t));
    e1000_write_reg(base, E1000_RDH, 0);
    e1000_write_reg(base, E1000_RDT, E1000_RX_DESC_COUNT - 1);

    /* Set MAC filter to accept everything for now */
    for (int i = 0; i < 128; i++) {
        e1000_write_reg(base, E1000_MTA + i * 4, 0);
    }

    /* Enable receiver */
    uint32_t rctl = e1000_read_reg(base, E1000_RCTL);
    rctl |= RCTL_EN | RCTL_BAM;
    rctl |= RCTL_BSIZE_2048;
    e1000_write_reg(base, E1000_RCTL, rctl);

    klog_info("E1000: RX initialized with %d descriptors", E1000_RX_DESC_COUNT);
    return 0;
}

int e1000_rx_packet(uint64_t base, void *buf, size_t max_len) {
    uint32_t tail = (e1000_read_reg(base, E1000_RDT) + 1) % E1000_RX_DESC_COUNT;

    if (!(g_e1000_rx_ring[tail].status & 0x01)) {
        return 0; /* No packet */
    }

    size_t len = g_e1000_rx_ring[tail].length;
    if (len > max_len) len = max_len;
    memcpy(buf, g_e1000_rx_buffers[tail], len);

    /* Reclaim descriptor */
    g_e1000_rx_ring[tail].status = 0;
    e1000_write_reg(base, E1000_RDT, tail);

    return (int)len;
}

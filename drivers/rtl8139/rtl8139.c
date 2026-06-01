/* gOS Kernel — Realtek RTL8139 NIC Implementation
 * Reference: https://wiki.osdev.org/RTL8139
 */

#include "drivers/rtl8139/rtl8139.h"
#include "drivers/nic/nic.h"
#include "drivers/pci/pci.h"
#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"

static uint16_t g_rtl8139_base = 0;
static uint8_t *g_rtl8139_rx_buf = NULL;
static uint8_t *g_rtl8139_tx_buf[4] = {NULL, NULL, NULL, NULL};
static int __attribute__((used)) g_rtl8139_tx_cur = 0;

static inline uint8_t rtl_read8(uint16_t reg) {
    return inb(g_rtl8139_base + reg);
}

static inline uint16_t rtl_read16(uint16_t reg) {
    return inw(g_rtl8139_base + reg);
}

static inline uint32_t rtl_read32(uint16_t reg) {
    return inl(g_rtl8139_base + reg);
}

static inline void rtl_write8(uint16_t reg, uint8_t val) {
    outb(g_rtl8139_base + reg, val);
}

static inline void rtl_write16(uint16_t reg, uint16_t val) {
    outw(g_rtl8139_base + reg, val);
}

static inline void rtl_write32(uint16_t reg, uint32_t val) {
    outl(g_rtl8139_base + reg, val);
}

void rtl8139_init(void) {
    klog_info("RTL8139 driver initializing");

    if (!rtl8139_probe()) {
        klog_warn("RTL8139: no device found");
        return;
    }

    klog_info("RTL8139: controller found, I/O at 0x%04x", g_rtl8139_base);

    /* Software reset */
    rtl_write8(RTL8139_CMD, RTL8139_CMD_RST);
    for (int i = 0; i < 10000; i++) {
        if (!(rtl_read8(RTL8139_CMD) & RTL8139_CMD_RST)) break;
        io_wait();
    }

    /* Read MAC */
    uint8_t mac[6];
    for (int i = 0; i < 6; i++) {
        mac[i] = rtl_read8(RTL8139_MAC0 + i);
    }

    /* Allocate receive buffer (8KB + 16 bytes padding, must be 16-bit aligned) */
    g_rtl8139_rx_buf = kzalloc(RTL8139_RX_BUF_SIZE + 16 + 0x1000);
    if (!g_rtl8139_rx_buf) {
        klog_err("RTL8139: failed to allocate RX buffer");
        return;
    }
    uintptr_t rx_phys = VIRT_TO_PHYS(((uintptr_t)g_rtl8139_rx_buf + 0x1000) & ~0xFFF);
    rtl_write32(RTL8139_RX_BUF, (uint32_t)rx_phys);

    /* Allocate TX buffers */
    for (int i = 0; i < 4; i++) {
        g_rtl8139_tx_buf[i] = kzalloc(RTL8139_TX_BUF_SIZE + 0x1000);
        if (!g_rtl8139_tx_buf[i]) {
            klog_err("RTL8139: failed to allocate TX buffer %d", i);
            return;
        }
        rtl_write32(RTL8139_TX_ADDR0 + i * 4,
                    (uint32_t)VIRT_TO_PHYS(((uintptr_t)g_rtl8139_tx_buf[i] + 0x1000) & ~0xFFF));
    }

    /* Set RX config: accept broadcast, multicast, physical match, promiscuous off */
    rtl_write32(RTL8139_RX_CFG, RTL8139_RX_CFG_AB | RTL8139_RX_CFG_AM |
                RTL8139_RX_CFG_APM | RTL8139_RX_CFG_WRAP | RTL8139_RX_CFG_8K);

    /* Set TX config */
    rtl_write32(RTL8139_TX_CFG, 0x00000600); /* Early TX threshold */

    /* Enable RX/TX */
    rtl_write8(RTL8139_CMD, RTL8139_CMD_RX_EN | RTL8139_CMD_TX_EN);

    /* Enable interrupts */
    rtl_write16(RTL8139_IMR, 0x0005);

    nic_register("rtl8139", mac, NULL, NULL, NULL, NULL);
    klog_info("RTL8139: initialized, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

bool rtl8139_probe(void) {
    const pci_device_t *dev = pci_find_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID);
    if (!dev) return false;

    g_rtl8139_base = (uint16_t)pci_get_bar_address(dev, 0);
    return true;
}

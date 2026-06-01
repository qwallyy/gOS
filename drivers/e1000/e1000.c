/* gOS Kernel — Intel E1000 NIC Implementation
 * Reference: https://wiki.osdev.org/Intel_8254x
 */

#include "drivers/e1000/e1000.h"
#include "drivers/nic/nic.h"
#include "drivers/pci/pci.h"
#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"

uint64_t g_e1000_base = 0;

static uint16_t e1000_read_eeprom(uint8_t addr) {
    e1000_write_reg(g_e1000_base, E1000_EERD, 1 | ((uint32_t)addr << 8));
    for (int i = 0; i < 10000; i++) {
        uint32_t val = e1000_read_reg(g_e1000_base, E1000_EERD);
        if (val & 0x10) {
            return (uint16_t)((val >> 16) & 0xFFFF);
        }
    }
    return 0;
}

static bool e1000_get_mac(uint8_t *mac) {
    if (!g_e1000_base) return false;
    uint32_t ral = e1000_read_reg(g_e1000_base, E1000_RAL);
    uint32_t rah = e1000_read_reg(g_e1000_base, E1000_RAH);
    if (rah & 0x80000000) {
        mac[0] = ral & 0xFF;
        mac[1] = (ral >> 8) & 0xFF;
        mac[2] = (ral >> 16) & 0xFF;
        mac[3] = (ral >> 24) & 0xFF;
        mac[4] = rah & 0xFF;
        mac[5] = (rah >> 8) & 0xFF;
        return true;
    }
    /* Try EEPROM */
    uint16_t eeword = e1000_read_eeprom(0);
    if (eeword) {
        mac[0] = eeword & 0xFF;
        mac[1] = (eeword >> 8) & 0xFF;
        eeword = e1000_read_eeprom(1);
        mac[2] = eeword & 0xFF;
        mac[3] = (eeword >> 8) & 0xFF;
        eeword = e1000_read_eeprom(2);
        mac[4] = eeword & 0xFF;
        mac[5] = (eeword >> 8) & 0xFF;
        return true;
    }
    return false;
}

void e1000_init(void) {
    klog_info("E1000 driver initializing");

    if (!e1000_probe()) {
        klog_warn("E1000: no device found");
        return;
    }

    klog_info("E1000: controller found, MMIO at 0x%016llx",
              (unsigned long long)g_e1000_base);

    /* Read MAC address */
    uint8_t mac[6] = {0};
    if (e1000_get_mac(mac)) {
        klog_info("E1000: MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        klog_warn("E1000: failed to read MAC address");
    }

    /* Set link up in CTRL */
    uint32_t ctrl = e1000_read_reg(g_e1000_base, E1000_CTRL);
    ctrl |= (1U << 6); /* SLU - Set Link Up */
    ctrl &= ~(1U << 3); /* Clear LRST */
    ctrl &= ~(1U << 31); /* Clear RST */
    e1000_write_reg(g_e1000_base, E1000_CTRL, ctrl);

    /* Register with NIC abstraction */
    nic_register("e1000", mac, (void *)g_e1000_base, NULL, NULL, NULL);
    klog_info("E1000: initialization complete");
}

bool e1000_probe(void) {
    const pci_device_t *dev = pci_find_device(E1000_VENDOR_ID, E1000_DEVICE_ID);
    if (!dev) return false;

    uint64_t bar0 = pci_get_bar_address(dev, 0);
    if (!bar0) return false;

    g_e1000_base = bar0;

    /* Map BAR0 if MMIO */
    if (pci_is_mmio_bar(dev, 0)) {
        for (uint64_t off = 0; off < 0x20000; off += PAGE_SIZE) {
            vmm_map_page(g_e1000_base + off, g_e1000_base + off,
                         PAGE_PRESENT | PAGE_WRITABLE | PAGE_NOCACHE);
        }
    }
    return true;
}

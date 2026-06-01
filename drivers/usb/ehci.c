/* gOS Kernel — EHCI Host Controller Implementation
 * Reference: https://wiki.osdev.org/EHCI
 */

#include "drivers/usb/ehci.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"

static inline uint32_t ehci_cap_read(uint32_t base, uint32_t reg) {
    return mmio_read32(base + reg);
}

static inline uint32_t ehci_op_read(uint32_t base, uint32_t reg) {
    uint8_t caplen = ehci_cap_read(base, EHCI_CAPLENGTH) & 0xFF;
    return mmio_read32(base + caplen + reg);
}

static inline void ehci_op_write(uint32_t base, uint32_t reg, uint32_t val) {
    uint8_t caplen = ehci_cap_read(base, EHCI_CAPLENGTH) & 0xFF;
    mmio_write32(base + caplen + reg, val);
}

void ehci_init(uint32_t base) {
    klog_info("EHCI controller initializing at 0x%08x", base);

    uint32_t hcs = ehci_cap_read(base, EHCI_HCSPARAMS);
    uint32_t hcc = ehci_cap_read(base, EHCI_HCCPARAMS);
    uint8_t ports = hcs & 0x0F;

    klog_info("EHCI: %u ports, 64-bit=%d", ports, !!(hcc & 0x01));

    ehci_reset(base);
    ehci_enable_ports(base);

    klog_info("EHCI: controller ready");
}

bool ehci_reset(uint32_t base) {
    ehci_op_write(base, EHCI_USBCMD, EHCI_CMD_RESET);
    for (int i = 0; i < 100000; i++) {
        if (!(ehci_op_read(base, EHCI_USBCMD) & EHCI_CMD_RESET)) {
            return true;
        }
        io_wait();
    }
    klog_warn("EHCI: reset timeout");
    return false;
}

void ehci_enable_ports(uint32_t base) {
    uint32_t hcs = ehci_cap_read(base, EHCI_HCSPARAMS);
    uint8_t ports = hcs & 0x0F;
    for (uint8_t i = 1; i <= ports; i++) {
        uint32_t portsc = ehci_op_read(base, EHCI_PORTSC1 + (i - 1) * 4);
        if (portsc & 0x01) { /* Device connected */
            klog_info("EHCI: port %u has device", i);
        }
    }
}

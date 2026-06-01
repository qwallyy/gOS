/* gOS Kernel — OHCI Host Controller Implementation
 * Reference: https://wiki.osdev.org/OHCI
 */

#include "drivers/usb/ohci.h"
#include "mm/vmm/vmm.h"
#include "gOS/klog.h"

static inline uint32_t ohci_read(uint32_t base, uint32_t reg) {
    return mmio_read32(base + reg);
}

static inline void ohci_write(uint32_t base, uint32_t reg, uint32_t val) {
    mmio_write32(base + reg, val);
}

void ohci_init(uint32_t base) {
    klog_info("OHCI controller initializing at 0x%08x", base);

    uint32_t rev = ohci_read(base, OHCI_REVISION);
    klog_info("OHCI: revision %u.%u", (rev >> 4) & 0xF, rev & 0xF);

    /* Reset controller */
    ohci_reset(base);

    /* Set to operational state */
    ohci_set_operational(base);

    klog_info("OHCI: controller operational");
}

bool ohci_reset(uint32_t base) {
    ohci_write(base, OHCI_CMDSTATUS, OHCI_CMD_RESET);
    for (int i = 0; i < 100000; i++) {
        if (!(ohci_read(base, OHCI_CMDSTATUS) & OHCI_CMD_RESET)) {
            return true;
        }
        io_wait();
    }
    klog_warn("OHCI: reset timeout");
    return false;
}

void ohci_set_operational(uint32_t base) {
    uint32_t ctrl = ohci_read(base, OHCI_CONTROL);
    ctrl &= ~OHCI_CTRL_HCFS;
    ctrl |= OHCI_USB_OPERATIONAL;
    ohci_write(base, OHCI_CONTROL, ctrl);
}

/* gOS Kernel — UHCI Host Controller Implementation
 * Reference: https://wiki.osdev.org/UHCI
 */

#include "drivers/usb/uhci.h"
#include "gOS/klog.h"

void uhci_init(uint16_t base) {
    klog_info("UHCI controller initializing at 0x%04x", base);

    /* Global reset */
    outw(base + UHCI_REG_USBCMD, UHCI_CMD_GRESET);
    for (int i = 0; i < 10000; i++) io_wait();
    outw(base + UHCI_REG_USBCMD, 0);

    /* Host controller reset */
    outw(base + UHCI_REG_USBCMD, UHCI_CMD_HCRESET);
    for (int i = 0; i < 10000; i++) {
        if (!(inw(base + UHCI_REG_USBCMD) & UHCI_CMD_HCRESET)) break;
        io_wait();
    }

    /* Clear status */
    outw(base + UHCI_REG_USBSTS, 0xFFFF);

    klog_info("UHCI: controller reset complete");
    uhci_dump_ports(base);
}

bool uhci_reset(uint16_t base) {
    outw(base + UHCI_REG_USBCMD, UHCI_CMD_HCRESET);
    for (int i = 0; i < 10000; i++) {
        if (!(inw(base + UHCI_REG_USBCMD) & UHCI_CMD_HCRESET)) {
            return true;
        }
        io_wait();
    }
    return false;
}

void uhci_dump_ports(uint16_t base) {
    uint16_t port1 = inw(base + UHCI_REG_PORT1);
    uint16_t port2 = inw(base + UHCI_REG_PORT2);
    klog_info("UHCI Port1: 0x%04x (conn=%d en=%d)", port1,
              !!(port1 & UHCI_PORT_CONN), !!(port1 & UHCI_PORT_EN));
    klog_info("UHCI Port2: 0x%04x (conn=%d en=%d)", port2,
              !!(port2 & UHCI_PORT_CONN), !!(port2 & UHCI_PORT_EN));
}

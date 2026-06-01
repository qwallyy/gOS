/* gOS Kernel — USB Host Controller Implementation
 * Reference: https://wiki.osdev.org/USB
 */

#include "drivers/usb/usb.h"
#include "drivers/pci/pci.h"
#include "gOS/klog.h"

void usb_init(void) {
    klog_info("USB subsystem initializing");

    /* Look for UHCI (0x0C03, prog_if 0x00), OHCI (0x0C03, prog_if 0x10),
     * EHCI (0x0C03, prog_if 0x20), XHCI (0x0C03, prog_if 0x30) */
    for (size_t i = 0; i < g_pci_device_count; i++) {
        if (g_pci_devices[i].class_code == 0x0C && g_pci_devices[i].subclass == 0x03) {
            const char *type = "Unknown";
            switch (g_pci_devices[i].prog_if) {
                case 0x00: type = "UHCI"; break;
                case 0x10: type = "OHCI"; break;
                case 0x20: type = "EHCI"; break;
                case 0x30: type = "XHCI"; break;
            }
            klog_info("USB: found %s controller at %02x:%02x.%x",
                      type, g_pci_devices[i].bus, g_pci_devices[i].device,
                      g_pci_devices[i].function);
        }
    }
}

bool usb_probe_controller(uint16_t vendor, uint16_t device) {
    (void)vendor; (void)device;
    klog_warn("USB controller probe: not yet implemented");
    return false;
}

/* gOS Kernel — USB Host Controller Driver
 * Reference: https://wiki.osdev.org/USB
 *            https://wiki.osdev.org/UHCI
 *            https://wiki.osdev.org/OHCI
 *            https://wiki.osdev.org/EHCI
 *            https://wiki.osdev.org/XHCI
 */

#ifndef GOS_USB_H
#define GOS_USB_H

#include "gOS/types.h"

#define USB_CLASS_HUB       0x09
#define USB_CLASS_HID       0x03
#define USB_CLASS_MASS      0x08
#define USB_CLASS_VIDEO     0x0E

void usb_init(void);
bool usb_probe_controller(uint16_t vendor, uint16_t device);

#endif /* GOS_USB_H */

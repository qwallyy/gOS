/* gOS Kernel — OHCI Host Controller
 * Reference: https://wiki.osdev.org/OHCI
 */

#ifndef GOS_OHCI_H
#define GOS_OHCI_H

#include "gOS/types.h"

#define OHCI_REVISION       0x00
#define OHCI_CONTROL        0x04
#define OHCI_CMDSTATUS      0x08
#define OHCI_INTRSTATUS     0x0C
#define OHCI_INTRENABLE     0x10
#define OHCI_HCCA           0x18
#define OHCI_FM_INTERVAL    0x34

#define OHCI_CTRL_HCFS      0x000000C0
#define OHCI_USB_RESET      0x00000000
#define OHCI_USB_RESUME     0x00000040
#define OHCI_USB_OPERATIONAL 0x00000080
#define OHCI_USB_SUSPEND    0x000000C0

#define OHCI_CMD_RESET      0x00000001

void ohci_init(uint32_t base);
bool ohci_reset(uint32_t base);
void ohci_set_operational(uint32_t base);

#endif /* GOS_OHCI_H */

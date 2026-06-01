/* gOS Kernel — EHCI Host Controller
 * Reference: https://wiki.osdev.org/EHCI
 */

#ifndef GOS_EHCI_H
#define GOS_EHCI_H

#include "gOS/types.h"

#define EHCI_CAPLENGTH      0x00
#define EHCI_HCSPARAMS      0x04
#define EHCI_HCCPARAMS      0x08

#define EHCI_USBCMD         0x00
#define EHCI_USBSTS         0x04
#define EHCI_USBINTR        0x08
#define EHCI_FRINDEX        0x0C
#define EHCI_CTRLDSSEGMENT  0x10
#define EHCI_PERIODICLISTBASE 0x14
#define EHCI_ASYNCLISTADDR  0x18
#define EHCI_CONFIGFLAG     0x40
#define EHCI_PORTSC1        0x44
#define EHCI_PORTSC2        0x48

#define EHCI_CMD_RUN        0x00000001
#define EHCI_CMD_RESET       0x00000002

#define EHCI_STS_HCHALTED   0x00001000

void ehci_init(uint32_t base);
bool ehci_reset(uint32_t base);
void ehci_enable_ports(uint32_t base);

#endif /* GOS_EHCI_H */

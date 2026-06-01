/* gOS Kernel — UHCI Host Controller
 * Reference: https://wiki.osdev.org/UHCI
 */

#ifndef GOS_UHCI_H
#define GOS_UHCI_H

#include "gOS/types.h"

#define UHCI_REG_USBCMD     0x00
#define UHCI_REG_USBSTS     0x02
#define UHCI_REG_USBINTR    0x04
#define UHCI_REG_FRNUM      0x06
#define UHCI_REG_FLBASEADD  0x08
#define UHCI_REG_SOF        0x0C
#define UHCI_REG_PORT1      0x10
#define UHCI_REG_PORT2      0x12

#define UHCI_CMD_RUN        0x0001
#define UHCI_CMD_HCRESET    0x0002
#define UHCI_CMD_GRESET     0x0004
#define UHCI_CMD_GSUSPEND   0x0008
#define UHCI_CMD_GRESUME    0x0010

#define UHCI_STS_USBINT     0x0001
#define UHCI_STS_ERROR      0x0002
#define UHCI_STS_RESUME     0x0004
#define UHCI_STS_HCERR      0x0008
#define UHCI_STS_HCHALTED   0x0010

#define UHCI_PORT_CONN      0x0001
#define UHCI_PORT_EN        0x0002
#define UHCI_PORT_RESET     0x0100

void uhci_init(uint16_t base);
bool uhci_reset(uint16_t base);
void uhci_dump_ports(uint16_t base);

#endif /* GOS_UHCI_H */

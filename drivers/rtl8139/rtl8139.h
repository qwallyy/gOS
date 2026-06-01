/* gOS Kernel — Realtek RTL8139 NIC Driver
 * Reference: https://wiki.osdev.org/RTL8139
 */

#ifndef GOS_RTL8139_H
#define GOS_RTL8139_H

#include "gOS/types.h"

#define RTL8139_VENDOR_ID   0x10EC
#define RTL8139_DEVICE_ID   0x8139

/* Register offsets */
#define RTL8139_MAC0        0x00
#define RTL8139_MAR0        0x08
#define RTL8139_TX_STATUS0  0x10
#define RTL8139_TX_ADDR0    0x20
#define RTL8139_RX_BUF      0x30
#define RTL8139_CMD         0x37
#define RTL8139_RX_PTR      0x38
#define RTL8139_IMR         0x3C
#define RTL8139_ISR         0x3E
#define RTL8139_TX_CFG      0x40
#define RTL8139_RX_CFG      0x44
#define RTL8139_TIMER       0x48
#define RTL8139_MPC         0x4C
#define RTL8139_9346CR      0x50
#define RTL8139_CONFIG0     0x51
#define RTL8139_CONFIG1     0x52
#define RTL8139_MSR         0x58
#define RTL8139_BMCR        0x62
#define RTL8139_BMSR        0x64

#define RTL8139_CMD_RX_EN   0x08
#define RTL8139_CMD_TX_EN   0x04
#define RTL8139_CMD_RST     0x10

#define RTL8139_RX_CFG_AAP  (1U << 0)
#define RTL8139_RX_CFG_APM  (1U << 1)
#define RTL8139_RX_CFG_AM   (1U << 2)
#define RTL8139_RX_CFG_AB   (1U << 3)
#define RTL8139_RX_CFG_WRAP (1U << 7)
#define RTL8139_RX_CFG_8K   (0x0U << 11)
#define RTL8139_RX_CFG_16K  (0x1U << 11)
#define RTL8139_RX_CFG_32K  (0x2U << 11)
#define RTL8139_RX_CFG_64K  (0x3U << 11)
#define RTL8139_RX_CFG_FTH  (0x2U << 13)
#define RTL8139_RX_CFG_RBLEN (0x0U << 11)

#define RTL8139_RX_BUF_SIZE 8192
#define RTL8139_TX_BUF_SIZE 1536

void rtl8139_init(void);
bool rtl8139_probe(void);

#endif /* GOS_RTL8139_H */

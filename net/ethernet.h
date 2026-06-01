/* gOS Kernel — Ethernet Layer
 * Reference: https://wiki.osdev.org/Ethernet
 */

#ifndef GOS_ETHERNET_H
#define GOS_ETHERNET_H

#include "gOS/types.h"

#define ETH_TYPE_IP         0x0800
#define ETH_TYPE_ARP        0x0806
#define ETH_TYPE_IPV6       0x86DD

#define ETH_HDR_LEN         14
#define ETH_MAX_FRAME       1518

typedef struct eth_header {
    uint8_t  dst_mac[6];
    uint8_t  src_mac[6];
    uint16_t ethertype;
} PACKED eth_header_t;

void ethernet_init(void);
int  ethernet_send(const uint8_t *dst_mac, uint16_t type, const void *data, size_t len);
void ethernet_receive(const void *frame, size_t len);
void ethernet_get_mac(uint8_t *mac);

#endif /* GOS_ETHERNET_H */

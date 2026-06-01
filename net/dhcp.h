/* gOS Kernel — DHCP Client
 * Reference: https://wiki.osdev.org/DHCP
 */

#ifndef GOS_DHCP_H
#define GOS_DHCP_H

#include "gOS/types.h"

#define DHCP_OP_REQUEST     1
#define DHCP_OP_REPLY       2
#define DHCP_MAGIC_COOKIE   0x63825363

typedef struct dhcp_packet {
    uint8_t  op;
    uint8_t  htype;
    uint8_t  hlen;
    uint8_t  hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t  chaddr[16];
    uint8_t  sname[64];
    uint8_t  file[128];
    uint32_t magic;
    uint8_t  options[];
} PACKED dhcp_packet_t;

void dhcp_init(void);
void dhcp_discover(void);
void dhcp_receive(const void *packet, size_t len);
bool dhcp_has_lease(void);
uint32_t dhcp_get_ip(void);
uint32_t dhcp_get_gateway(void);
uint32_t dhcp_get_dns(void);

#endif /* GOS_DHCP_H */

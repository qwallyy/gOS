/* gOS Kernel — IPv4 Layer
 * Reference: https://wiki.osdev.org/Internet_Protocol
 */

#ifndef GOS_IP_H
#define GOS_IP_H

#include "gOS/types.h"

#define IP_PROTOCOL_ICMP    1
#define IP_PROTOCOL_TCP     6
#define IP_PROTOCOL_UDP     17

#define IP_VERSION_4        4

typedef struct ip_header {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_fragment;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} PACKED ip_header_t;

void ip_init(void);
void ip_receive(const void *packet, size_t len);
int  ip_send(uint32_t dst, uint8_t protocol, const void *data, size_t len);
uint16_t ip_checksum(const void *data, size_t len);

#endif /* GOS_IP_H */

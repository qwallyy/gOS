/* gOS Kernel — IPv6 Layer (stub)
 * Reference: https://wiki.osdev.org/IPv6
 */

#ifndef GOS_IPV6_H
#define GOS_IPV6_H

#include "gOS/types.h"

#define IPV6_HDR_LEN        40
#define IPV6_ADDR_LEN       16

typedef struct ipv6_header {
    uint32_t version_tc_fl; /* version(4), traffic class(8), flow label(20) */
    uint16_t payload_length;
    uint8_t  next_header;
    uint8_t  hop_limit;
    uint8_t  src_addr[16];
    uint8_t  dst_addr[16];
} PACKED ipv6_header_t;

void ipv6_init(void);
void ipv6_receive(const void *packet, size_t len);

#endif /* GOS_IPV6_H */

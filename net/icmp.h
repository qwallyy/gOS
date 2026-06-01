/* gOS Kernel — ICMP (Internet Control Message Protocol)
 * Reference: https://wiki.osdev.org/ICMP
 */

#ifndef GOS_ICMP_H
#define GOS_ICMP_H

#include "gOS/types.h"

#define ICMP_ECHO_REPLY     0
#define ICMP_DEST_UNREACH   3
#define ICMP_ECHO_REQUEST   8
#define ICMP_TIME_EXCEEDED  11

typedef struct icmp_header {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
} PACKED icmp_header_t;

void icmp_init(void);
void icmp_receive(const void *packet, size_t len, uint32_t src_ip);
void icmp_send_echo_request(uint32_t dst_ip, uint16_t id, uint16_t seq);

#endif /* GOS_ICMP_H */

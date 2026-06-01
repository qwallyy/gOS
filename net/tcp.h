/* gOS Kernel — TCP (Transmission Control Protocol)
 * Reference: https://wiki.osdev.org/TCP
 */

#ifndef GOS_TCP_H
#define GOS_TCP_H

#include "gOS/types.h"

#define TCP_HDR_LEN_MIN     20
#define TCP_FLAG_FIN        0x01
#define TCP_FLAG_SYN        0x02
#define TCP_FLAG_RST        0x04
#define TCP_FLAG_PSH        0x08
#define TCP_FLAG_ACK        0x10
#define TCP_FLAG_URG        0x20

#define TCP_STATE_CLOSED        0
#define TCP_STATE_LISTEN        1
#define TCP_STATE_SYN_SENT      2
#define TCP_STATE_SYN_RECEIVED  3
#define TCP_STATE_ESTABLISHED   4
#define TCP_STATE_FIN_WAIT_1    5
#define TCP_STATE_FIN_WAIT_2    6
#define TCP_STATE_CLOSE_WAIT    7
#define TCP_STATE_CLOSING       8
#define TCP_STATE_LAST_ACK      9
#define TCP_STATE_TIME_WAIT     10

typedef struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t  data_offset; /* upper 4 bits = header len in dwords */
    uint8_t  flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent;
} PACKED tcp_header_t;

void tcp_init(void);
void tcp_receive(const void *packet, size_t len, uint32_t src_ip);

#endif /* GOS_TCP_H */

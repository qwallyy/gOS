/* gOS Kernel — UDP (User Datagram Protocol)
 * Reference: https://wiki.osdev.org/UDP
 */

#ifndef GOS_UDP_H
#define GOS_UDP_H

#include "gOS/types.h"

#define UDP_HDR_LEN     8

typedef struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} PACKED udp_header_t;

void udp_init(void);
void udp_receive(const void *packet, size_t len, uint32_t src_ip);
int  udp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
              const void *data, size_t len);

#endif /* GOS_UDP_H */

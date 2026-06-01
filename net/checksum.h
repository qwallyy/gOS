/* gOS Kernel — Network Checksum Utilities */

#ifndef GOS_NET_CHECKSUM_H
#define GOS_NET_CHECKSUM_H

#include "gOS/types.h"

uint16_t net_checksum_fold(uint32_t sum);
uint16_t net_checksum_add(const void *data, size_t len, uint32_t init);
uint16_t net_tcp_checksum(const void *tcp_hdr, size_t tcp_len,
                          uint32_t src_ip, uint32_t dst_ip);
uint16_t net_udp_checksum(const void *udp_hdr, size_t udp_len,
                          uint32_t src_ip, uint32_t dst_ip);

#endif /* GOS_NET_CHECKSUM_H */

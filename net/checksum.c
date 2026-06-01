/* gOS Kernel — Network Checksum Implementation
 * Reference: RFC 1071 — Computing the Internet Checksum
 */

#include "net/checksum.h"
#include "lib/string.h"

uint16_t net_checksum_fold(uint32_t sum) {
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (uint16_t)(~sum);
}

uint16_t net_checksum_add(const void *data, size_t len, uint32_t init) {
    const uint16_t *words = data;
    uint32_t sum = init;
    while (len > 1) {
        sum += *words++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(const uint8_t *)words;
    }
    return net_checksum_fold(sum);
}

uint16_t net_tcp_checksum(const void *tcp_hdr, size_t tcp_len,
                          uint32_t src_ip, uint32_t dst_ip) {
    /* Pseudo-header: src IP, dst IP, zero, protocol (6), TCP length */
    uint32_t sum = 0;
    sum += (src_ip >> 16) & 0xFFFF;
    sum += src_ip & 0xFFFF;
    sum += (dst_ip >> 16) & 0xFFFF;
    sum += dst_ip & 0xFFFF;
    sum += 6; /* TCP protocol */
    sum += tcp_len & 0xFFFF;
    sum += (tcp_len >> 16) & 0xFFFF;
    return net_checksum_add(tcp_hdr, tcp_len, sum);
}

uint16_t net_udp_checksum(const void *udp_hdr, size_t udp_len,
                          uint32_t src_ip, uint32_t dst_ip) {
    /* Pseudo-header: src IP, dst IP, zero, protocol (17), UDP length */
    uint32_t sum = 0;
    sum += (src_ip >> 16) & 0xFFFF;
    sum += src_ip & 0xFFFF;
    sum += (dst_ip >> 16) & 0xFFFF;
    sum += dst_ip & 0xFFFF;
    sum += 17; /* UDP protocol */
    sum += udp_len & 0xFFFF;
    sum += (udp_len >> 16) & 0xFFFF;
    return net_checksum_add(udp_hdr, udp_len, sum);
}

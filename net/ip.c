/* gOS Kernel — IPv4 Implementation
 * Reference: https://wiki.osdev.org/Internet_Protocol
 */

#include "net/ip.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

static uint32_t g_ip_addr = 0;
static uint32_t __attribute__((used)) g_ip_netmask = 0xFFFFFF00; /* /24 */
static uint32_t g_ip_gateway = 0;

void ip_init(void) {
    klog_info("IP layer initialized");
    /* Default address: 10.0.2.15 (common QEMU user-mode default) */
    g_ip_addr = (10U) | (0U << 8) | (2U << 16) | (15U << 24);
    g_ip_gateway = (10U) | (0U << 8) | (2U << 16) | (2U << 24);
}

uint16_t ip_checksum(const void *data, size_t len) {
    const uint16_t *ptr = data;
    uint32_t sum = 0;
    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(const uint8_t *)ptr;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (uint16_t)(~sum);
}

void ip_receive(const void *packet, size_t len) {
    if (len < sizeof(ip_header_t)) return;

    const ip_header_t *ip = (const ip_header_t *)packet;
    uint8_t version = (ip->version_ihl >> 4) & 0x0F;
    if (version != IP_VERSION_4) {
        klog_debug("IP: received non-IPv4 packet (version=%u)", version);
        return;
    }

    uint8_t ihl = (ip->version_ihl & 0x0F) * 4;
    uint16_t total_len = ((ip->total_length & 0xFF) << 8) | ((ip->total_length >> 8) & 0xFF);
    uint8_t proto = ip->protocol;

    klog_debug("IP packet: src=%d.%d.%d.%d dst=%d.%d.%d.%d proto=%u len=%u",
               (ip->src_ip & 0xFF), (ip->src_ip >> 8) & 0xFF,
               (ip->src_ip >> 16) & 0xFF, (ip->src_ip >> 24) & 0xFF,
               (ip->dst_ip & 0xFF), (ip->dst_ip >> 8) & 0xFF,
               (ip->dst_ip >> 16) & 0xFF, (ip->dst_ip >> 24) & 0xFF,
               proto, total_len);

    const void *payload = (const uint8_t *)packet + ihl;
    size_t payload_len = total_len - ihl;
    (void)payload;
    (void)payload_len;

    switch (proto) {
        case IP_PROTOCOL_ICMP:
            klog_debug("IP: ICMP packet received");
            break;
        case IP_PROTOCOL_TCP:
            klog_debug("IP: TCP packet received");
            break;
        case IP_PROTOCOL_UDP:
            klog_debug("IP: UDP packet received");
            break;
        default:
            klog_debug("IP: unknown protocol %u", proto);
            break;
    }
}

int ip_send(uint32_t dst, uint8_t protocol, const void *data, size_t len) {
    (void)dst; (void)protocol; (void)data; (void)len;
    klog_debug("IP send: not implemented yet");
    return -1;
}

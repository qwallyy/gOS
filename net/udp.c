/* gOS Kernel — UDP Implementation
 * Reference: https://wiki.osdev.org/UDP
 */

#include "net/udp.h"
#include "net/ip.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

void udp_init(void) {
    klog_info("UDP initialized");
}

void udp_receive(const void *packet, size_t len, uint32_t src_ip) {
    if (len < sizeof(udp_header_t)) return;
    const udp_header_t *udp = packet;
    uint16_t src_port = ((udp->src_port & 0xFF) << 8) | ((udp->src_port >> 8) & 0xFF);
    uint16_t dst_port = ((udp->dst_port & 0xFF) << 8) | ((udp->dst_port >> 8) & 0xFF);
    uint16_t data_len = ((udp->length & 0xFF) << 8) | ((udp->length >> 8) & 0xFF);

    klog_debug("UDP packet: %d.%d.%d.%d:%u -> :%u, len=%u",
               (src_ip & 0xFF), (src_ip >> 8) & 0xFF,
               (src_ip >> 16) & 0xFF, (src_ip >> 24) & 0xFF,
               src_port, dst_port, data_len);
}

int udp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
             const void *data, size_t len) {
    (void)dst_ip; (void)src_port; (void)dst_port; (void)data; (void)len;
    klog_warn("UDP send: stub (IP send not fully implemented)");
    return -1;
}

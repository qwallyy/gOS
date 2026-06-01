/* gOS Kernel — ICMP Implementation
 * Reference: https://wiki.osdev.org/ICMP
 */

#include "net/icmp.h"
#include "net/ip.h"
#include "net/ethernet.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

void icmp_init(void) {
    klog_info("ICMP initialized");
}

void icmp_receive(const void *packet, size_t len, uint32_t src_ip) {
    if (len < sizeof(icmp_header_t)) return;
    const icmp_header_t *icmp = packet;

    klog_debug("ICMP: type=%u code=%u from %d.%d.%d.%d",
               icmp->type, icmp->code,
               (src_ip & 0xFF), (src_ip >> 8) & 0xFF,
               (src_ip >> 16) & 0xFF, (src_ip >> 24) & 0xFF);

    switch (icmp->type) {
        case ICMP_ECHO_REQUEST: {
            klog_info("ICMP echo request received, sending reply");
            /* For now, just log — full reply requires IP layer encapsulation */
            break;
        }
        case ICMP_ECHO_REPLY: {
            klog_info("ICMP echo reply received (seq=%u)",
                      ((icmp->sequence & 0xFF) << 8) | ((icmp->sequence >> 8) & 0xFF));
            break;
        }
        case ICMP_DEST_UNREACH:
            klog_warn("ICMP destination unreachable code=%u", icmp->code);
            break;
        case ICMP_TIME_EXCEEDED:
            klog_warn("ICMP time exceeded code=%u", icmp->code);
            break;
        default:
            klog_debug("ICMP unhandled type=%u", icmp->type);
            break;
    }
}

void icmp_send_echo_request(uint32_t dst_ip, uint16_t id, uint16_t seq) {
    (void)dst_ip; (void)id; (void)seq;
    klog_warn("ICMP echo request: stub (IP send not fully implemented)");
}

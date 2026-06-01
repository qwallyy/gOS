/* gOS Kernel — TCP Implementation (stub)
 * Reference: https://wiki.osdev.org/TCP
 */

#include "net/tcp.h"
#include "gOS/klog.h"

void tcp_init(void) {
    klog_info("TCP initialized (stub)");
}

void tcp_receive(const void *packet, size_t len, uint32_t src_ip) {
    (void)packet; (void)len; (void)src_ip;
    klog_debug("TCP packet received (not handled in stub build)");
}

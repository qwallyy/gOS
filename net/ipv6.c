/* gOS Kernel — IPv6 Implementation (stub)
 * Reference: https://wiki.osdev.org/IPv6
 */

#include "net/ipv6.h"
#include "gOS/klog.h"

void ipv6_init(void) {
    klog_info("IPv6 layer initialized (stub)");
}

void ipv6_receive(const void *packet, size_t len) {
    (void)packet; (void)len;
    klog_debug("IPv6 packet received (stub, no processing)");
}

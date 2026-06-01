/* gOS Kernel — DNS Client Implementation (stub)
 * Reference: https://wiki.osdev.org/DNS
 */

#include "net/dns.h"
#include "net/udp.h"
#include "gOS/klog.h"
#include "lib/string.h"

void dns_init(void) {
    klog_info("DNS client initialized (stub)");
}

bool dns_query(const char *hostname, uint32_t *out_ip) {
    (void)hostname;
    klog_warn("dns_query: stub (no network stack)");
    if (out_ip) *out_ip = 0;
    return false;
}

void dns_receive(const void *packet, size_t len) {
    (void)packet; (void)len;
    klog_debug("DNS packet received (stub)");
}

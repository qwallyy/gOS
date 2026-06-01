/* gOS Kernel — NDP Implementation (stub)
 * Reference: https://wiki.osdev.org/IPv6
 */

#include "net/ndp.h"
#include "gOS/klog.h"

void ndp_init(void) {
    klog_info("NDP initialized (IPv6 neighbor discovery stub)");
}

void ndp_receive(const void *packet, size_t len) {
    (void)packet; (void)len;
    klog_debug("NDP packet received (stub)");
}

void ndp_send_neighbor_solicit(const uint8_t *target) {
    (void)target;
    klog_warn("ndp_send_neighbor_solicit: stub");
}

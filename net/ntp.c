/* gOS Kernel — NTP Client Implementation (stub)
 * Reference: https://wiki.osdev.org/NTP
 */

#include "net/ntp.h"
#include "gOS/klog.h"

static uint64_t g_ntp_time = 0;

void ntp_init(void) {
    klog_info("NTP client initialized (stub)");
}

bool ntp_sync(const char *server_ip) {
    (void)server_ip;
    klog_warn("ntp_sync: stub (no UDP send path)");
    return false;
}

void ntp_receive(const void *packet, size_t len) {
    (void)packet; (void)len;
    klog_debug("NTP packet received (stub)");
}

uint64_t ntp_get_time(void) {
    return g_ntp_time;
}

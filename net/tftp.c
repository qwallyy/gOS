/* gOS Kernel — TFTP Client Implementation (stub)
 * Reference: https://wiki.osdev.org/TFTP
 */

#include "net/tftp.h"
#include "gOS/klog.h"

void tftp_init(void) {
    klog_info("TFTP client initialized (stub)");
}

bool tftp_read_file(const char *server_ip, const char *filename,
                    void *buf, size_t max_size, size_t *out_size) {
    (void)server_ip; (void)filename; (void)buf; (void)max_size;
    klog_warn("tftp_read_file: stub");
    if (out_size) *out_size = 0;
    return false;
}

void tftp_receive(const void *packet, size_t len) {
    (void)packet; (void)len;
    klog_debug("TFTP packet received (stub)");
}

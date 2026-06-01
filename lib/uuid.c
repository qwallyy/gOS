/* gOS Kernel — UUID Implementation
 * Reference: RFC 4122
 */

#include "lib/uuid.h"
#include "lib/ctype.h"
#include "gOS/klog.h"
#include "lib/stdio.h"
#include "lib/string.h"

void uuid_generate_random(uuid_t *out) {
    if (!out) return;
    /* Simple random fill for stub */
    static uint32_t seed = 0xDEADBEEF;
    uint32_t *words = (uint32_t *)out;
    for (int i = 0; i < 4; i++) {
        seed = seed * 1664525 + 1013904223;
        words[i] = seed;
    }
    /* Set version (4) and variant (2) */
    out->time_hi_and_version = (out->time_hi_and_version & 0x0FFF) | 0x4000;
    out->clock_seq_hi_and_reserved = (out->clock_seq_hi_and_reserved & 0x3F) | 0x80;
}

void uuid_to_string(const uuid_t *uuid, char *buf, size_t size) {
    if (!uuid || !buf || size < 37) return;
    snprintf(buf, size, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid->time_low, uuid->time_mid, uuid->time_hi_and_version,
             uuid->clock_seq_hi_and_reserved, uuid->clock_seq_low,
             uuid->node[0], uuid->node[1], uuid->node[2],
             uuid->node[3], uuid->node[4], uuid->node[5]);
}

bool uuid_parse(const char *str, uuid_t *out) {
    if (!str || !out || strlen(str) < 36) return false;
    /* Simplified parsing - not fully implemented */
    (void)str; (void)out;
    return false;
}

bool uuid_equal(const uuid_t *a, const uuid_t *b) {
    if (!a || !b) return false;
    return memcmp(a, b, sizeof(uuid_t)) == 0;
}

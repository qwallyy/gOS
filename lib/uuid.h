/* gOS Kernel — UUID Generation
 * Reference: RFC 4122
 */

#ifndef GOS_UUID_H
#define GOS_UUID_H

#include "gOS/types.h"

typedef struct uuid {
    uint32_t time_low;
    uint16_t time_mid;
    uint16_t time_hi_and_version;
    uint8_t  clock_seq_hi_and_reserved;
    uint8_t  clock_seq_low;
    uint8_t  node[6];
} uuid_t;

void uuid_generate_random(uuid_t *out);
void uuid_to_string(const uuid_t *uuid, char *buf, size_t size);
bool uuid_parse(const char *str, uuid_t *out);
bool uuid_equal(const uuid_t *a, const uuid_t *b);

#endif /* GOS_UUID_H */

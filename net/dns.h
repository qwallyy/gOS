/* gOS Kernel — DNS Client Stub
 * Reference: https://wiki.osdev.org/DNS
 */

#ifndef GOS_DNS_H
#define GOS_DNS_H

#include "gOS/types.h"

#define DNS_PORT            53
#define DNS_TYPE_A          1
#define DNS_TYPE_AAAA       28
#define DNS_TYPE_CNAME      5
#define DNS_CLASS_IN        1

typedef struct dns_header {
    uint16_t id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answer_rrs;
    uint16_t authority_rrs;
    uint16_t additional_rrs;
} PACKED dns_header_t;

typedef struct dns_question {
    uint16_t type;
    uint16_t class;
} PACKED dns_question_t;

typedef struct dns_answer {
    uint16_t name_ptr;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t data_len;
    uint8_t  data[];
} PACKED dns_answer_t;

void dns_init(void);
bool dns_query(const char *hostname, uint32_t *out_ip);
void dns_receive(const void *packet, size_t len);

#endif /* GOS_DNS_H */

/* gOS Kernel — NDP (Neighbor Discovery Protocol) for IPv6
 * Reference: https://wiki.osdev.org/IPv6
 */

#ifndef GOS_NDP_H
#define GOS_NDP_H

#include "gOS/types.h"

#define NDP_ROUTER_SOLICIT      133
#define NDP_ROUTER_ADVERT       134
#define NDP_NEIGHBOR_SOLICIT    135
#define NDP_NEIGHBOR_ADVERT     136

typedef struct ndp_neighbor_solicit {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint32_t reserved;
    uint8_t  target_addr[16];
} PACKED ndp_neighbor_solicit_t;

void ndp_init(void);
void ndp_receive(const void *packet, size_t len);
void ndp_send_neighbor_solicit(const uint8_t *target);

#endif /* GOS_NDP_H */

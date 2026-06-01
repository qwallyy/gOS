/* gOS Kernel — IP Routing Table
 * Reference: https://wiki.osdev.org/Routing
 */

#ifndef GOS_ROUTING_H
#define GOS_ROUTING_H

#include "gOS/types.h"

#define ROUTE_TABLE_SIZE    16

typedef struct route_entry {
    uint32_t dest;
    uint32_t netmask;
    uint32_t gateway;
    uint32_t iface_ip;
    uint8_t  metric;
    bool     valid;
} route_entry_t;

void routing_init(void);
bool routing_add(uint32_t dest, uint32_t netmask, uint32_t gateway, uint8_t metric);
bool routing_remove(uint32_t dest, uint32_t netmask);
const route_entry_t *routing_lookup(uint32_t dest_ip);
void routing_dump(void);

#endif /* GOS_ROUTING_H */

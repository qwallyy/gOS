/* gOS Kernel — IP Routing Table Implementation */

#include "net/routing.h"
#include "gOS/klog.h"
#include "lib/string.h"

static route_entry_t g_route_table[ROUTE_TABLE_SIZE];

void routing_init(void) {
    klog_info("Routing table initialized");
    for (int i = 0; i < ROUTE_TABLE_SIZE; i++) {
        g_route_table[i].valid = false;
    }
}

bool routing_add(uint32_t dest, uint32_t netmask, uint32_t gateway, uint8_t metric) {
    for (int i = 0; i < ROUTE_TABLE_SIZE; i++) {
        if (!g_route_table[i].valid) {
            g_route_table[i].dest = dest;
            g_route_table[i].netmask = netmask;
            g_route_table[i].gateway = gateway;
            g_route_table[i].metric = metric;
            g_route_table[i].valid = true;
            klog_info("Route added: %d.%d.%d.%d/%d via %d.%d.%d.%d metric=%u",
                      (dest & 0xFF), (dest >> 8) & 0xFF,
                      (dest >> 16) & 0xFF, (dest >> 24) & 0xFF,
                      32 - __builtin_clz(netmask),
                      (gateway & 0xFF), (gateway >> 8) & 0xFF,
                      (gateway >> 16) & 0xFF, (gateway >> 24) & 0xFF,
                      metric);
            return true;
        }
    }
    klog_err("Routing table full");
    return false;
}

bool routing_remove(uint32_t dest, uint32_t netmask) {
    for (int i = 0; i < ROUTE_TABLE_SIZE; i++) {
        if (g_route_table[i].valid &&
            g_route_table[i].dest == dest &&
            g_route_table[i].netmask == netmask) {
            g_route_table[i].valid = false;
            return true;
        }
    }
    return false;
}

const route_entry_t *routing_lookup(uint32_t dest_ip) {
    const route_entry_t *best = NULL;
    int best_match = -1;
    for (int i = 0; i < ROUTE_TABLE_SIZE; i++) {
        if (!g_route_table[i].valid) continue;
        if ((dest_ip & g_route_table[i].netmask) == g_route_table[i].dest) {
            int match = 0;
            uint32_t mask = g_route_table[i].netmask;
            while (mask) { match++; mask &= mask - 1; }
            if (match > best_match) {
                best_match = match;
                best = &g_route_table[i];
            }
        }
    }
    return best;
}

void routing_dump(void) {
    klog_info("Routing table:");
    for (int i = 0; i < ROUTE_TABLE_SIZE; i++) {
        if (!g_route_table[i].valid) continue;
        route_entry_t *r = &g_route_table[i];
        klog_info("  %d.%d.%d.%d/%d -> %d.%d.%d.%d metric=%u",
                  (r->dest & 0xFF), (r->dest >> 8) & 0xFF,
                  (r->dest >> 16) & 0xFF, (r->dest >> 24) & 0xFF,
                  32 - __builtin_clz(r->netmask),
                  (r->gateway & 0xFF), (r->gateway >> 8) & 0xFF,
                  (r->gateway >> 16) & 0xFF, (r->gateway >> 24) & 0xFF,
                  r->metric);
    }
}

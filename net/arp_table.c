/* gOS Kernel — ARP Table Implementation */

#include "net/arp_table.h"
#include "gOS/klog.h"
#include "lib/string.h"

static arp_entry_t g_arp_table[ARP_TABLE_SIZE];

void arp_table_init(void) {
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        g_arp_table[i].valid = false;
    }
}

bool arp_table_add(uint32_t ip, const uint8_t *mac) {
    /* Update existing entry */
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (g_arp_table[i].valid && g_arp_table[i].ip == ip) {
            memcpy(g_arp_table[i].mac, mac, 6);
            return true;
        }
    }
    /* Add new entry */
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (!g_arp_table[i].valid) {
            g_arp_table[i].ip = ip;
            memcpy(g_arp_table[i].mac, mac, 6);
            g_arp_table[i].valid = true;
            return true;
        }
    }
    return false;
}

bool arp_table_lookup(uint32_t ip, uint8_t *mac) {
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (g_arp_table[i].valid && g_arp_table[i].ip == ip) {
            if (mac) memcpy(mac, g_arp_table[i].mac, 6);
            return true;
        }
    }
    return false;
}

void arp_table_remove(uint32_t ip) {
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (g_arp_table[i].valid && g_arp_table[i].ip == ip) {
            g_arp_table[i].valid = false;
            return;
        }
    }
}

void arp_table_dump(void) {
    klog_info("ARP table:");
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (!g_arp_table[i].valid) continue;
        klog_info("  %d.%d.%d.%d -> %02x:%02x:%02x:%02x:%02x:%02x",
                  (g_arp_table[i].ip & 0xFF),
                  (g_arp_table[i].ip >> 8) & 0xFF,
                  (g_arp_table[i].ip >> 16) & 0xFF,
                  (g_arp_table[i].ip >> 24) & 0xFF,
                  g_arp_table[i].mac[0], g_arp_table[i].mac[1], g_arp_table[i].mac[2],
                  g_arp_table[i].mac[3], g_arp_table[i].mac[4], g_arp_table[i].mac[5]);
    }
}

void arp_table_flush(void) {
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        g_arp_table[i].valid = false;
    }
}

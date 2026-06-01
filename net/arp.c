/* gOS Kernel — ARP Implementation
 * Reference: https://wiki.osdev.org/ARP
 */

#include "net/arp.h"
#include "net/ethernet.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

#define ARP_TABLE_SIZE  16

typedef struct arp_entry {
    uint32_t ip;
    uint8_t  mac[6];
    bool     valid;
    uint64_t timestamp;
} arp_entry_t;

static arp_entry_t g_arp_table[ARP_TABLE_SIZE];

void arp_init(void) {
    klog_info("ARP initialized");
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        g_arp_table[i].valid = false;
    }
}

void arp_send_request(uint32_t target_ip) {
    (void)target_ip;
    klog_debug("ARP request: not sending (no NIC up)");
}

void arp_send_reply(const uint8_t *target_mac, uint32_t target_ip) {
    (void)target_mac; (void)target_ip;
    klog_debug("ARP reply: not sending (no NIC up)");
}

void arp_receive(const void *packet, size_t len) {
    if (len < sizeof(arp_packet_t)) return;

    const arp_packet_t *arp = (const arp_packet_t *)packet;
    uint16_t hw_type  = ((arp->hw_type & 0xFF) << 8) | ((arp->hw_type >> 8) & 0xFF);
    uint16_t op       = ((arp->opcode & 0xFF) << 8) | ((arp->opcode >> 8) & 0xFF);

    if (hw_type != ARP_HW_TYPE_ETHER || arp->proto_len != 4) return;

    /* Store in cache */
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (!g_arp_table[i].valid) {
            g_arp_table[i].ip = arp->sender_ip;
            memcpy(g_arp_table[i].mac, arp->sender_mac, 6);
            g_arp_table[i].valid = true;
            break;
        }
    }

    if (op == ARP_OP_REQUEST) {
        klog_debug("ARP request received");
    } else if (op == ARP_OP_REPLY) {
        klog_debug("ARP reply received from %d.%d.%d.%d",
                   (arp->sender_ip & 0xFF),
                   (arp->sender_ip >> 8) & 0xFF,
                   (arp->sender_ip >> 16) & 0xFF,
                   (arp->sender_ip >> 24) & 0xFF);
    }
}

bool arp_lookup(uint32_t ip, uint8_t *mac) {
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (g_arp_table[i].valid && g_arp_table[i].ip == ip) {
            if (mac) memcpy(mac, g_arp_table[i].mac, 6);
            return true;
        }
    }
    return false;
}

void arp_dump_table(void) {
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

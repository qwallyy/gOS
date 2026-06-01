/* gOS Kernel — ARP (Address Resolution Protocol)
 * Reference: https://wiki.osdev.org/ARP
 */

#ifndef GOS_ARP_H
#define GOS_ARP_H

#include "gOS/types.h"

#define ARP_HW_TYPE_ETHER   1
#define ARP_PROTO_TYPE_IP   0x0800
#define ARP_OP_REQUEST      1
#define ARP_OP_REPLY        2

typedef struct arp_packet {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_len;
    uint8_t  proto_len;
    uint16_t opcode;
    uint8_t  sender_mac[6];
    uint32_t sender_ip;
    uint8_t  target_mac[6];
    uint32_t target_ip;
} PACKED arp_packet_t;

void arp_init(void);
void arp_send_request(uint32_t target_ip);
void arp_send_reply(const uint8_t *target_mac, uint32_t target_ip);
void arp_receive(const void *packet, size_t len);
bool arp_lookup(uint32_t ip, uint8_t *mac);
void arp_dump_table(void);

#endif /* GOS_ARP_H */

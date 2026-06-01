/* gOS Kernel — ARP Table Declarations */

#ifndef GOS_ARP_TABLE_H
#define GOS_ARP_TABLE_H

#include "gOS/types.h"

#define ARP_TABLE_SIZE      16

typedef struct arp_entry {
    uint32_t ip;
    uint8_t  mac[6];
    bool     valid;
    uint64_t timestamp;
} arp_entry_t;

void arp_table_init(void);
bool arp_table_add(uint32_t ip, const uint8_t *mac);
bool arp_table_lookup(uint32_t ip, uint8_t *mac);
void arp_table_remove(uint32_t ip);
void arp_table_dump(void);
void arp_table_flush(void);

#endif /* GOS_ARP_TABLE_H */

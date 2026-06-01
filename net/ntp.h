/* gOS Kernel — NTP Client Stub
 * Reference: https://wiki.osdev.org/NTP
 */

#ifndef GOS_NTP_H
#define GOS_NTP_H

#include "gOS/types.h"

#define NTP_PORT            123
#define NTP_EPOCH_OFFSET    2208988800ULL

typedef struct ntp_packet {
    uint8_t  li_vn_mode;
    uint8_t  stratum;
    uint8_t  poll;
    uint8_t  precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t ref_id;
    uint64_t ref_timestamp;
    uint64_t orig_timestamp;
    uint64_t recv_timestamp;
    uint64_t tx_timestamp;
} PACKED ntp_packet_t;

void ntp_init(void);
bool ntp_sync(const char *server_ip);
void ntp_receive(const void *packet, size_t len);
uint64_t ntp_get_time(void);

#endif /* GOS_NTP_H */

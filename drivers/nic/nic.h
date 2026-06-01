/* gOS Kernel — Network Interface Card (NIC) Abstraction
 * Reference: https://wiki.osdev.org/Network_Stack
 */

#ifndef GOS_NIC_H
#define GOS_NIC_H

#include "gOS/types.h"

#define NIC_MAX_INTERFACES      4
#define NIC_MAC_LEN             6
#define NIC_MTU                 1500
#define NIC_RX_BUFS             32
#define NIC_TX_BUFS             32

typedef struct nic_device {
    char name[16];
    uint8_t mac[NIC_MAC_LEN];
    uint32_t ip_addr;
    uint32_t netmask;
    uint32_t gateway;
    bool present;
    bool up;
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    void *private_data;
    int (*init)(struct nic_device *nic);
    int (*send)(struct nic_device *nic, const void *data, size_t len);
    int (*poll)(struct nic_device *nic, void *buf, size_t max_len);
} nic_device_t;

void nic_init(void);
bool nic_register(const char *name, uint8_t *mac, void *priv,
                  int (*init)(nic_device_t *),
                  int (*send)(nic_device_t *, const void *, size_t),
                  int (*poll)(nic_device_t *, void *, size_t));
bool nic_send_packet(const void *data, size_t len);
int  nic_poll(void *buf, size_t max_len);
void nic_dump(void);

extern nic_device_t g_nics[NIC_MAX_INTERFACES];
extern int g_nic_count;

#endif /* GOS_NIC_H */

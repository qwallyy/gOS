/* gOS Kernel — NIC Abstraction Implementation
 * Reference: https://wiki.osdev.org/Network_Stack
 */

#include "drivers/nic/nic.h"
#include "gOS/klog.h"
#include "lib/string.h"

nic_device_t g_nics[NIC_MAX_INTERFACES];
int g_nic_count = 0;

void nic_init(void) {
    klog_info("NIC subsystem initializing");
    for (int i = 0; i < NIC_MAX_INTERFACES; i++) {
        g_nics[i].present = false;
        g_nics[i].name[0] = '\0';
    }
}

bool nic_register(const char *name, uint8_t *mac, void *priv,
                  int (*init)(nic_device_t *),
                  int (*send)(nic_device_t *, const void *, size_t),
                  int (*poll)(nic_device_t *, void *, size_t)) {
    if (g_nic_count >= NIC_MAX_INTERFACES) return false;
    nic_device_t *nic = &g_nics[g_nic_count++];
    strncpy(nic->name, name, 15);
    nic->name[15] = '\0';
    memcpy(nic->mac, mac, NIC_MAC_LEN);
    nic->present = true;
    nic->up = false;
    nic->private_data = priv;
    nic->init = init;
    nic->send = send;
    nic->poll = poll;
    nic->rx_packets = 0;
    nic->tx_packets = 0;
    nic->rx_bytes = 0;
    nic->tx_bytes = 0;

    if (init) init(nic);
    nic->up = true;

    klog_info("NIC '%s' registered, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
              nic->name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return true;
}

bool nic_send_packet(const void *data, size_t len) {
    if (!data || len == 0) return false;
    for (int i = 0; i < g_nic_count; i++) {
        if (g_nics[i].present && g_nics[i].up && g_nics[i].send) {
            if (g_nics[i].send(&g_nics[i], data, len) >= 0) {
                g_nics[i].tx_packets++;
                g_nics[i].tx_bytes += len;
                return true;
            }
        }
    }
    return false;
}

int nic_poll(void *buf, size_t max_len) {
    if (!buf) return -1;
    for (int i = 0; i < g_nic_count; i++) {
        if (g_nics[i].present && g_nics[i].up && g_nics[i].poll) {
            int ret = g_nics[i].poll(&g_nics[i], buf, max_len);
            if (ret > 0) {
                g_nics[i].rx_packets++;
                g_nics[i].rx_bytes += ret;
                return ret;
            }
        }
    }
    return 0;
}

void nic_dump(void) {
    klog_info("NIC interfaces:");
    for (int i = 0; i < g_nic_count; i++) {
        nic_device_t *n = &g_nics[i];
        klog_info("  %s: MAC=%02x:%02x:%02x:%02x:%02x:%02x up=%d rx=%llu tx=%llu",
                  n->name, n->mac[0], n->mac[1], n->mac[2],
                  n->mac[3], n->mac[4], n->mac[5],
                  n->up,
                  (unsigned long long)n->rx_packets,
                  (unsigned long long)n->tx_packets);
    }
}

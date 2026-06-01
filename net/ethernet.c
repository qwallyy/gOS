/* gOS Kernel — Ethernet Layer Implementation
 * Reference: https://wiki.osdev.org/Ethernet
 */

#include "net/ethernet.h"
#include "net/arp.h"
#include "net/ip.h"
#include "drivers/nic/nic.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

void ethernet_init(void) {
    klog_info("Ethernet layer initialized");
}

int ethernet_send(const uint8_t *dst_mac, uint16_t type, const void *data, size_t len) {
    if (len > ETH_MAX_FRAME - ETH_HDR_LEN) return -1;
    if (!g_nic_count) return -1;

    size_t frame_len = ETH_HDR_LEN + len;
    uint8_t *frame = kmalloc(frame_len);
    if (!frame) return -1;

    eth_header_t *hdr = (eth_header_t *)frame;
    memcpy(hdr->dst_mac, dst_mac, 6);
    ethernet_get_mac(hdr->src_mac);
    hdr->ethertype = ((type & 0xFF) << 8) | ((type >> 8) & 0xFF); /* big-endian */
    memcpy(frame + ETH_HDR_LEN, data, len);

    bool ok = nic_send_packet(frame, frame_len);
    kfree(frame);
    return ok ? (int)frame_len : -1;
}

void ethernet_receive(const void *frame, size_t len) {
    if (len < ETH_HDR_LEN) return;

    const eth_header_t *hdr = (const eth_header_t *)frame;
    uint16_t type = ((hdr->ethertype & 0xFF) << 8) | ((hdr->ethertype >> 8) & 0xFF);

    const void *payload = (const uint8_t *)frame + ETH_HDR_LEN;
    size_t payload_len = len - ETH_HDR_LEN;

    switch (type) {
        case ETH_TYPE_ARP:
            arp_receive(payload, payload_len);
            break;
        case ETH_TYPE_IP:
            ip_receive(payload, payload_len);
            break;
        default:
            klog_debug("Ethernet: unhandled type 0x%04x", type);
            break;
    }
}

void ethernet_get_mac(uint8_t *mac) {
    if (g_nic_count > 0) {
        memcpy(mac, g_nics[0].mac, 6);
    } else {
        memset(mac, 0, 6);
    }
}

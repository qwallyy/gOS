/* gOS Kernel — DHCP Client Implementation
 * Reference: https://wiki.osdev.org/DHCP
 */

#include "net/dhcp.h"
#include "net/udp.h"
#include "net/ip.h"
#include "net/ethernet.h"
#include "gOS/klog.h"
#include "lib/string.h"

static uint32_t g_dhcp_ip = 0;
static uint32_t g_dhcp_gateway = 0;
static uint32_t g_dhcp_dns = 0;
static bool g_dhcp_leased = false;

void dhcp_init(void) {
    klog_info("DHCP client initialized");
}

void dhcp_discover(void) {
    klog_info("DHCP: sending DISCOVER");
    /* For stub, use a static IP */
    g_dhcp_ip = (10U) | (0U << 8) | (2U << 16) | (15U << 24);
    g_dhcp_gateway = (10U) | (0U << 8) | (2U << 16) | (2U << 24);
    g_dhcp_dns = (8U) | (8U << 8) | (8U << 16) | (8U << 24);
    g_dhcp_leased = true;
    klog_info("DHCP: using static IP %d.%d.%d.%d",
              (g_dhcp_ip & 0xFF), (g_dhcp_ip >> 8) & 0xFF,
              (g_dhcp_ip >> 16) & 0xFF, (g_dhcp_ip >> 24) & 0xFF);
}

void dhcp_receive(const void *packet, size_t len) {
    (void)packet; (void)len;
    klog_debug("DHCP packet received (stub)");
}

bool dhcp_has_lease(void) {
    return g_dhcp_leased;
}

uint32_t dhcp_get_ip(void) {
    return g_dhcp_ip;
}

uint32_t dhcp_get_gateway(void) {
    return g_dhcp_gateway;
}

uint32_t dhcp_get_dns(void) {
    return g_dhcp_dns;
}

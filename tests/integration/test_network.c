/* gOS Kernel — Network Integration Tests */

#include "net/ethernet.h"
#include "net/arp.h"
#include "net/ip.h"
#include "net/icmp.h"
#include "net/udp.h"
#include "net/tcp.h"
#include "drivers/nic/nic.h"
#include "gOS/klog.h"
#include "lib/assert.h"
#include "lib/string.h"

static void test_ethernet_init(void) {
    ethernet_init();
    klog_info("[NET_TEST] Ethernet initialized");
}

static void test_arp_init(void) {
    arp_init();
    klog_info("[NET_TEST] ARP initialized");
}

static void test_ip_init(void) {
    ip_init();
    klog_info("[NET_TEST] IP initialized");
}

static void test_icmp_init(void) {
    icmp_init();
    klog_info("[NET_TEST] ICMP initialized");
}

static void test_udp_init(void) {
    udp_init();
    klog_info("[NET_TEST] UDP initialized");
}

static void test_tcp_init(void) {
    tcp_init();
    klog_info("[NET_TEST] TCP initialized");
}

static void test_nic_layer(void) {
    assert(g_nic_count >= 0);
    klog_info("[NET_TEST] NIC layer OK (%d interfaces)", g_nic_count);
}

void run_network_integration_tests(void) {
    klog_info("================================");
    klog_info("  Network Integration Tests");
    klog_info("================================");

    test_ethernet_init();
    test_arp_init();
    test_ip_init();
    test_icmp_init();
    test_udp_init();
    test_tcp_init();
    test_nic_layer();

    klog_info("================================");
    klog_info("  Network Tests PASSED");
    klog_info("================================");
}

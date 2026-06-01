/* gOS Kernel — Software Watchdog Implementation
 * Reference: https://wiki.osdev.org/Watchdog
 */

#include "drivers/watchdog/watchdog.h"
#include "drivers/pit/pit.h"
#include "gOS/klog.h"
#include "kernel/panic.h"

static bool g_watchdog_enabled = false;
static uint64_t g_watchdog_last_feed = 0;
static uint32_t g_watchdog_timeout = WATCHDOG_TIMEOUT_MS;

void watchdog_init(void) {
    klog_info("Software watchdog initialized (timeout=%u ms)", g_watchdog_timeout);
}

void watchdog_feed(void) {
    g_watchdog_last_feed = pit_get_ticks() * (1000 / PIT_HZ);
}

void watchdog_enable(void) {
    g_watchdog_enabled = true;
    watchdog_feed();
    klog_info("Watchdog enabled");
}

void watchdog_disable(void) {
    g_watchdog_enabled = false;
    klog_info("Watchdog disabled");
}

bool watchdog_is_enabled(void) {
    return g_watchdog_enabled;
}

void watchdog_set_timeout(uint32_t ms) {
    g_watchdog_timeout = ms;
}

/* Call from timer tick or scheduler to check timeout */
void watchdog_check(void) {
    if (!g_watchdog_enabled) return;
    uint64_t now = pit_get_ticks() * (1000 / PIT_HZ);
    if (now - g_watchdog_last_feed > g_watchdog_timeout) {
        klog_emerg("WATCHDOG TIMEOUT! System hung for %llu ms",
                   (unsigned long long)(now - g_watchdog_last_feed));
        kernel_panic(__FILE__, __LINE__, "Watchdog timeout");
    }
}

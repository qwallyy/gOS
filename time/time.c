/* gOS Kernel — Timekeeping Implementation
 * Reference: https://wiki.osdev.org/Programmable_Interval_Timer
 */

#include "time/time.h"
#include "drivers/pit/pit.h"
#include "gOS/klog.h"

void time_init(void) {
    klog_info("Timekeeping initialized");
}

uint64_t time_get_ms(void) {
    /* Each PIT tick is 10ms at 100Hz */
    return pit_get_ticks() * (1000 / PIT_HZ);
}

uint64_t time_get_us(void) {
    return time_get_ms() * 1000;
}

void time_sleep_ms(uint64_t ms) {
    pit_sleep_ms((uint32_t)ms);
}

void time_get_timespec(timespec_t *ts) {
    if (!ts) return;
    uint64_t ms = time_get_ms();
    ts->sec = ms / 1000;
    ts->nsec = (ms % 1000) * 1000000;
}

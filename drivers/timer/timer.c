/* gOS Kernel — Unified Timer Abstraction Implementation
 * Wraps PIT, HPET, and LAPIC timer sources.
 */

#include "drivers/timer/timer.h"
#include "drivers/pit/pit.h"
#include "drivers/hpet/hpet.h"
#include "drivers/rtc/rtc.h"
#include "apic/apic.h"
#include "gOS/klog.h"

static timer_source_t g_timer_source = TIMER_SOURCE_PIT;
static void (*g_timer_callback)(void) = NULL;

void timer_init(void) {
    klog_info("Timer subsystem initializing");

    if (hpet_is_available()) {
        g_timer_source = TIMER_SOURCE_HPET;
        klog_info("Timer: using HPET");
    } else if (lapic_get_id() != 0xFF) {
        g_timer_source = TIMER_SOURCE_LAPIC;
        klog_info("Timer: using LAPIC timer");
    } else {
        g_timer_source = TIMER_SOURCE_PIT;
        klog_info("Timer: using PIT (fallback)");
    }
}

void timer_set_source(timer_source_t source) {
    g_timer_source = source;
    klog_info("Timer source switched to %d", source);
}

timer_source_t timer_get_source(void) {
    return g_timer_source;
}

uint64_t timer_get_ticks(void) {
    switch (g_timer_source) {
        case TIMER_SOURCE_HPET:     return hpet_get_counter();
        case TIMER_SOURCE_LAPIC:    return lapic_read(0x390); /* LAPIC_CURR_CNT */
        case TIMER_SOURCE_PIT:
        default:                    return pit_get_ticks();
    }
}

uint64_t timer_get_frequency(void) {
    switch (g_timer_source) {
        case TIMER_SOURCE_HPET:     return hpet_get_frequency();
        case TIMER_SOURCE_LAPIC:    return 0; /* Unknown without calibration */
        case TIMER_SOURCE_PIT:
        default:                    return PIT_HZ;
    }
}

void timer_sleep_ms(uint64_t ms) {
    switch (g_timer_source) {
        case TIMER_SOURCE_HPET:     hpet_sleep_us(ms * 1000); break;
        case TIMER_SOURCE_PIT:
        default:                    pit_sleep_ms((uint32_t)ms); break;
    }
}

void timer_sleep_us(uint64_t us) {
    switch (g_timer_source) {
        case TIMER_SOURCE_HPET:     hpet_sleep_us(us); break;
        case TIMER_SOURCE_PIT:
        default:                    pit_sleep_ms((uint32_t)(us / 1000) + 1); break;
    }
}

void timer_register_callback(void (*callback)(void)) {
    g_timer_callback = callback;
}

void timer_unregister_callback(void) {
    g_timer_callback = NULL;
}

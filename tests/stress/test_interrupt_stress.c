/* gOS Kernel — Interrupt Stress Test
 * Rapidly triggers timer ticks and verifies stability.
 */

#include "drivers/pit/pit.h"
#include "gOS/klog.h"
#include "lib/assert.h"

static void test_timer_stability(void) {
    klog_info("[STRESS] Timer stability: 500ms");
    uint64_t t1 = pit_get_ticks();
    pit_sleep_ms(500);
    uint64_t t2 = pit_get_ticks();
    uint64_t diff = (t2 - t1) * (1000 / PIT_HZ);
    assert(diff >= 450 && diff <= 550);
    klog_info("[STRESS] Timer diff = %llu ms (OK)", (unsigned long long)diff);
}

static void test_yield_stress(void) {
    klog_info("[STRESS] Yield stress: 1000 yields");
    uint64_t ticks_before = pit_get_ticks();
    for (int i = 0; i < 1000; i++) {
        proc_yield();
    }
    uint64_t ticks_after = pit_get_ticks();
    klog_info("[STRESS] 1000 yields took %llu ticks",
              (unsigned long long)(ticks_after - ticks_before));
}

void run_interrupt_stress_tests(void) {
    klog_info("================================");
    klog_info("  Interrupt Stress Tests");
    klog_info("================================");

    test_timer_stability();
    test_yield_stress();

    klog_info("================================");
    klog_info("  Interrupt Stress PASSED");
    klog_info("================================");
}

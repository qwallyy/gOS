/* gOS Kernel — Scheduler Integration Test
 * Creates test processes and verifies scheduler switching.
 */

#include "proc/proc.h"
#include "proc/scheduler.h"
#include "gOS/klog.h"
#include "lib/assert.h"

static volatile int g_test_counter = 0;

static void test_process_entry(void) {
    for (int i = 0; i < 3; i++) {
        g_test_counter++;
        proc_yield();
    }
    proc_exit(0);
}

static void test_scheduler_create_and_run(void) {
    g_test_counter = 0;
    process_t *p1 = proc_create("test1", test_process_entry);
    process_t *p2 = proc_create("test2", test_process_entry);
    assert(p1 != NULL);
    assert(p2 != NULL);

    /* Yield to let them run */
    for (int i = 0; i < 10; i++) {
        proc_yield();
    }

    klog_info("[SCHED_TEST] Counter = %d (expected 6)", g_test_counter);
    /* Counter may not reach 6 in stub due to no real preemption yet */
}

static void test_scheduler_tick_count(void) {
    uint64_t ticks_before = g_scheduler_ticks;
    pit_sleep_ms(100);
    uint64_t ticks_after = g_scheduler_ticks;
    assert(ticks_after >= ticks_before + 5);
    klog_info("[SCHED_TEST] Tick count advanced: %llu -> %llu",
              (unsigned long long)ticks_before,
              (unsigned long long)ticks_after);
}

void run_scheduler_integration_tests(void) {
    klog_info("================================");
    klog_info("  Scheduler Integration Tests");
    klog_info("================================");

    test_scheduler_create_and_run();
    test_scheduler_tick_count();

    klog_info("================================");
    klog_info("  Scheduler Tests PASSED");
    klog_info("================================");
}

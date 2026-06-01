/* gOS Kernel — Unit Test Runner
 * Run all unit tests during kernel boot in debug mode.
 */

#include "gOS/klog.h"

extern void run_string_tests(void);
extern void run_pmm_tests(void);
extern void run_heap_tests(void);
extern void run_sort_tests(void);

void run_all_unit_tests(void) {
    klog_info("================================");
    klog_info("  Running Unit Tests");
    klog_info("================================");

    klog_info("Testing string utilities...");
    run_string_tests();
    klog_info("String tests: PASSED");

    klog_info("Testing PMM...");
    run_pmm_tests();
    klog_info("PMM tests: PASSED");

    klog_info("Testing heap...");
    run_heap_tests();
    klog_info("Heap tests: PASSED");

    klog_info("Testing sort...");
    run_sort_tests();
    klog_info("Sort tests: PASSED");

    klog_info("================================");
    klog_info("  All Unit Tests PASSED");
    klog_info("================================");
}

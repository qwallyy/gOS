/* gOS Kernel — Boot Integration Test
 * Verifies basic boot-time invariants after kernel initialization.
 */

#include "gOS/kernel.h"
#include "gOS/klog.h"
#include "hal/gdt/gdt.h"
#include "hal/idt/idt.h"
#include "hal/tss/tss.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "mm/heap/heap.h"
#include "lib/assert.h"

static void test_gdt_loaded(void) {
    gdt_ptr_t ptr;
    gdt_get_current(&ptr);
    assert(ptr.base != 0);
    assert(ptr.limit > 0);
    klog_info("[BOOT_TEST] GDT loaded: base=0x%llx limit=%u",
              (unsigned long long)ptr.base, ptr.limit);
}

static void test_idt_loaded(void) {
    idt_ptr_t ptr;
    idt_get_current(&ptr);
    assert(ptr.base != 0);
    assert(ptr.limit == 256 * 16 - 1);
    klog_info("[BOOT_TEST] IDT loaded: base=0x%llx limit=%u",
              (unsigned long long)ptr.base, ptr.limit);
}

static void test_memory_available(void) {
    uintptr_t total = pmm_get_total_memory();
    uintptr_t free = pmm_get_free_memory();
    assert(total > 0);
    assert(free > 0);
    klog_info("[BOOT_TEST] Memory: total=%llu KiB, free=%llu KiB",
              (unsigned long long)(total / 1024),
              (unsigned long long)(free / 1024));
}

static void test_heap_operational(void) {
    void *p = kmalloc(4096);
    assert(p != NULL);
    kfree(p);
    klog_info("[BOOT_TEST] Heap operational");
}

static void test_paging_active(void) {
    uint64_t cr3_val = read_cr3();
    assert(cr3_val != 0);
    klog_info("[BOOT_TEST] Paging active, CR3=0x%llx",
              (unsigned long long)cr3_val);
}

static void test_long_mode(void) {
    uint64_t efer = rdmsr(0xC0000080);
    assert((efer & (1ULL << 10)) != 0); /* LMA bit */
    klog_info("[BOOT_TEST] Long mode active (EFER.LMA=1)");
}

void run_boot_integration_tests(void) {
    klog_info("================================");
    klog_info("  Boot Integration Tests");
    klog_info("================================");

    test_gdt_loaded();
    test_idt_loaded();
    test_memory_available();
    test_heap_operational();
    test_paging_active();
    test_long_mode();

    klog_info("================================");
    klog_info("  Boot Integration Tests PASSED");
    klog_info("================================");
}

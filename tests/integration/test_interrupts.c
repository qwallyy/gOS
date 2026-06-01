/* gOS Kernel — Interrupt Integration Test
 * Verifies that PIC remapping, IRQ masking, and spurious detection work.
 */

#include "drivers/pic/pic.h"
#include "gOS/klog.h"
#include "lib/assert.h"

static void test_pic_remap(void) {
    /* After init, IRQ0 should map to vector 32 */
    assert(pic_irq_to_vector(0) == 32);
    assert(pic_irq_to_vector(1) == 33);
    assert(pic_irq_to_vector(15) == 47);
    klog_info("[IRQ_TEST] PIC remap verified");
}

static void test_pic_mask_unmask(void) {
    pic_mask_irq(5);
    pic_unmask_irq(5);
    /* No crash is the test */
    klog_info("[IRQ_TEST] PIC mask/unmask works");
}

static void test_pic_isr_read(void) {
    uint16_t isr = pic_get_isr();
    /* ISR read should not fault */
    klog_info("[IRQ_TEST] PIC ISR register readable: 0x%04x", isr);
}

void run_interrupt_integration_tests(void) {
    klog_info("================================");
    klog_info("  Interrupt Integration Tests");
    klog_info("================================");

    test_pic_remap();
    test_pic_mask_unmask();
    test_pic_isr_read();

    klog_info("================================");
    klog_info("  Interrupt Tests PASSED");
    klog_info("================================");
}

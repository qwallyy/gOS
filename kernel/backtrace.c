/* gOS Kernel — Stack Backtrace Implementation
 * Reference: https://wiki.osdev.org/Stack_Trace
 */

#include "kernel/backtrace.h"
#include "gOS/kernel.h"
#include "gOS/klog.h"
#include "lib/stdio.h"

void backtrace_print(void) {
    uint64_t addrs[BACKTRACE_MAX_DEPTH];
    size_t depth = 0;
    backtrace_capture(addrs, BACKTRACE_MAX_DEPTH, &depth);

    klog_info("Stack backtrace (%zu frames):", depth);
    for (size_t i = 0; i < depth; i++) {
        char sym[64];
        backtrace_symbol(addrs[i], sym, sizeof(sym));
        klog_info("  [%2zu] 0x%016llx %s", i, (unsigned long long)addrs[i], sym);
    }
}

void backtrace_capture(uint64_t *addrs, size_t max_depth, size_t *out_depth) {
    if (!addrs || !out_depth) return;

    stack_frame_t *frame;
    __asm__ volatile ("movq %%rbp, %0" : "=r"(frame));

    size_t depth = 0;
    while (frame && depth < max_depth) {
        addrs[depth] = frame->rip;
        depth++;
        frame = frame->rbp;
        /* Safety: stop if RBP looks invalid (not in higher half) */
        if ((uintptr_t)frame < KERNEL_VBASE) break;
    }
    *out_depth = depth;
}

void backtrace_symbol(uint64_t addr, char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) return;
    /* No symbol table in kernel yet — show raw address */
    snprintf(buf, buf_size, "<unknown>");
    (void)addr;
}

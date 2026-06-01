/* gOS Kernel — System Calls Implementation
 * Reference: https://wiki.osdev.org/System_Calls
 *            https://wiki.osdev.org/SYSENTER
 */

#include "sys/syscall.h"
#include "proc/proc.h"
#include "mm/heap/heap.h"
#include "drivers/serial/serial.h"
#include "drivers/vga/vga.h"
#include "gOS/klog.h"
#include "gOS/kernel.h"

static syscall_handler_t g_syscall_table[SYSCALL_MAX];

static uint64_t sys_exit(uint64_t code, uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e) {
    proc_exit((int)code);
    return 0;
}

static uint64_t sys_fork(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e, uint64_t _f) {
    klog_warn("sys_fork: not implemented");
    return 0;
}

static uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count, uint64_t _a, uint64_t _b, uint64_t _c) {
    (void)fd; (void)buf; (void)count;
    klog_warn("sys_read: not implemented");
    return 0;
}

static uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count, uint64_t _a, uint64_t _b, uint64_t _c) {
    const char *str = (const char *)(buf + KERNEL_VBASE);
    (void)fd;
    for (size_t i = 0; i < count && str[i]; i++) {
        vga_putchar(str[i]);
        serial_putc(str[i]);
    }
    return count;
}

static uint64_t sys_open(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e, uint64_t _f) {
    klog_warn("sys_open: not implemented");
    return 0;
}

static uint64_t sys_close(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e, uint64_t _f) {
    return 0;
}

static uint64_t sys_sleep(uint64_t ms, uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e) {
    proc_sleep(ms);
    return 0;
}

static uint64_t sys_getpid(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e, uint64_t _f) {
    process_t *p = proc_get_current();
    return p ? p->pid : 0;
}

static uint64_t sys_yield(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e, uint64_t _f) {
    proc_yield();
    return 0;
}

static uint64_t sys_malloc(uint64_t size, uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e) {
    return (uint64_t)kmalloc(size);
}

static uint64_t sys_free(uint64_t ptr, uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e) {
    kfree((void *)ptr);
    return 0;
}

void syscall_init(void) {
    klog_info("Initializing syscall table (%d entries)", SYSCALL_MAX);
    for (int i = 0; i < SYSCALL_MAX; i++) {
        g_syscall_table[i] = NULL;
    }
    syscall_register(SYS_EXIT, sys_exit);
    syscall_register(SYS_FORK, sys_fork);
    syscall_register(SYS_READ, sys_read);
    syscall_register(SYS_WRITE, sys_write);
    syscall_register(SYS_OPEN, sys_open);
    syscall_register(SYS_CLOSE, sys_close);
    syscall_register(SYS_SLEEP, sys_sleep);
    syscall_register(SYS_GETPID, sys_getpid);
    syscall_register(SYS_YIELD, sys_yield);
    syscall_register(SYS_MALLOC, sys_malloc);
    syscall_register(SYS_FREE, sys_free);

    /* Set up SYSCALL/SYSRET MSRs */
    /* STAR: kernel CS = 0x08, user CS = 0x20+16 (compatibility) | 0x18 (long) */
    wrmsr(0xC0000081, ((uint64_t)0x08 << 32) | ((uint64_t)0x18 << 48));
    /* LSTAR: syscall handler address */
    extern void syscall_entry_asm(void);
    wrmsr(0xC0000082, (uint64_t)syscall_entry_asm);
    /* FMASK: mask IF during syscall */
    wrmsr(0xC0000084, 0x200);

    klog_info("Syscalls initialized, LSTAR set");
}

void syscall_register(uint64_t num, syscall_handler_t handler) {
    if (num < SYSCALL_MAX) {
        g_syscall_table[num] = handler;
    }
}

/* Called from SYSCALL/SYSRET entry (hal/msrs/syscall_asm.asm) */
void syscall_dispatch(uint64_t num) {
    klog_debug("SYSCALL dispatch: num=%llu", (unsigned long long)num);
}

/* Called from INT 0x50 / INT 0x80 (legacy syscall entry) */
void syscall_dispatch_legacy(void *registers_ptr) {
    /* registers_ptr points to the saved register structure on the stack.
     * For a real implementation, we would extract RAX (syscall number) and
     * the argument registers from this structure.
     * Minimal implementation: just log and return. */
    (void)registers_ptr;
    klog_debug("Legacy syscall via INT");
}

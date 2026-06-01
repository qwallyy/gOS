/* gOS Kernel — Main Kernel Entry Point
 * Reference: https://wiki.osdev.org/Bare_Bones
 */

#include "gOS/kernel.h"
#include "gOS/klog.h"
#include "hal/gdt/gdt.h"
#include "hal/idt/idt.h"
#include "hal/tss/tss.h"
#include "hal/cpuid/cpuid.h"
#include "hal/msrs/msrs.h"
#include "drivers/pic/pic.h"
#include "drivers/pit/pit.h"
#include "drivers/serial/serial.h"
#include "drivers/vga/vga.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/rtc/rtc.h"
#include "drivers/pci/pci.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "mm/heap/heap.h"
#include "proc/proc.h"
#include "proc/scheduler.h"
#include "fs/vfs/vfs.h"
#include "sys/syscall.h"
#include "net/ethernet.h"
#include "lib/string.h"
#include "lib/stdio.h"

extern kernel_info_t g_kernel_info;

static void banner(void) {
    klog_info("========================================");
    klog_info("  gOS Kernel v" GOS_VERSION_STRING);
    klog_info("  Built for x86-64, Multiboot2/GRUB");
    klog_info("========================================");
}

void kernel_early_init(void) {
    /* Multiboot2 info address is passed in RDI by long_mode.asm */
    /* We do NOT parse multiboot2 here; the boot code already set up basic paging. */

    /* Initialize serial first for klog output */
    serial_init();
    klog_init();

    banner();
    klog_info("kernel_early_init() starting...");

    /* CPU detection */
    cpuid_init();

    /* Memory management */
    pmm_init();
    vmm_init();
    heap_init();

    /* HAL */
    gdt_init();
    idt_init();
    tss_init();
    msrs_init();

    /* Interrupt controllers */
    pic_init();

    /* Drivers */
    pit_init();
    vga_init();
    keyboard_init();
    rtc_init();

    /* IRQ handler registration */
    extern void pic_register_default_handlers(void);
    pic_register_default_handlers();

    /* Enable interrupts now that everything is set up */
    sti();

    klog_info("kernel_early_init() complete");
}

void kernel_main(void) {
    klog_info("kernel_main() entered");

    /* Show some system info */
    pmm_dump_stats();
    heap_dump_stats();

    /* PCI enumeration */
    pci_init();

    /* Filesystem layer */
    vfs_init();

    /* Process / scheduler */
    proc_init();
    scheduler_init();

    /* Syscalls */
    syscall_init();

    /* Network stack init (if NIC found) */
    ethernet_init();

    klog_info("gOS kernel initialization complete!");
    klog_info("Press keys to test keyboard, or wait for timer ticks.");

    /* Main idle loop */
    for (;;) {
        if (keyboard_has_input()) {
            char c = keyboard_get_char();
            if (c) {
                vga_putchar(c);
                serial_putc(c);
            }
        }
        hlt();
    }
}

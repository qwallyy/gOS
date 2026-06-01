/* gOS Kernel — Power Management Implementation
 * Reference: https://wiki.osdev.org/Shutdown
 *            https://wiki.osdev.org/Reboot
 */

#include "power/power.h"
#include "acpi/acpi.h"
#include "gOS/klog.h"

static int g_power_method = POWER_OFF_VIRTUAL;

void power_init(void) {
    klog_info("Power management initializing");

    /* Check if ACPI supports power off */
    acpi_sdt_header_t *fadt = acpi_find_table("FACP");
    if (fadt) {
        g_power_method = POWER_OFF_ACPI;
        klog_info("Power: ACPI FADT found, using ACPI shutdown");
    } else {
        /* Try APM or virtual shutdown */
        g_power_method = POWER_OFF_VIRTUAL;
        klog_info("Power: no ACPI, using virtual shutdown");
    }
}

void power_shutdown(void) {
    klog_info("System shutting down...");

    switch (g_power_method) {
        case POWER_OFF_ACPI: {
            /* ACPI shutdown: write to PM1a_CNT_BLK with SLP_TYP and SLP_EN */
            klog_info("ACPI shutdown sequence (stub)");
            break;
        }
        case POWER_OFF_VIRTUAL:
        default: {
            /* QEMU/VirtualBox: use isa-debug-exit ioport */
            outb(0x501, 0x00); /* Bochs/QEMU debug exit */
            break;
        }
    }

    /* If all else fails, triple fault or halt */
    for (;;) {
        cli();
        hlt();
    }
}

void power_reboot(void) {
    klog_info("System rebooting...");

    /* Try keyboard controller reset */
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE); /* Pulse reset line low */
    io_wait();

    /* Try triple fault */
    __asm__ volatile (
        "pushq $0\n"
        "popfq\n"
        "lidt (%%eax)\n" /* Invalid IDT pointer */
        "int $3\n"
        ::: "memory"
    );

    for (;;) {
        cli();
        hlt();
    }
}

void power_halt(void) {
    klog_info("System halting. Halt and catch fire.");
    for (;;) {
        cli();
        hlt();
    }
}

bool power_can_shutdown(void) {
    return g_power_method != POWER_OFF_TRIPLE;
}

bool power_can_reboot(void) {
    return true;
}

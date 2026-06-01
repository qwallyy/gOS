/* gOS Kernel — Panic Handler
 * Reference: https://wiki.osdev.org/Exceptions
 */

#include "gOS/kernel.h"
#include "gOS/klog.h"
#include "drivers/serial/serial.h"
#include "drivers/vga/vga.h"
#include "lib/string.h"

NORETURN void kernel_panic(const char *file, int line, const char *reason) {
    cli();

    const char *msg = "*** KERNEL PANIC ***";

    /* Output to VGA */
    vga_set_color(VGA_WHITE, VGA_RED);
    vga_clear();
    vga_puts(msg);
    vga_putchar('\n');
    vga_puts("File: ");
    vga_puts(file);
    vga_putchar('\n');
    vga_puts("Line: ");
    vga_putdec(line);
    vga_putchar('\n');
    vga_puts("Reason: ");
    vga_puts(reason);
    vga_putchar('\n');

    /* Output to serial */
    serial_puts("\r\n");
    serial_puts(msg);
    serial_puts("\r\nFile: ");
    serial_puts(file);
    serial_puts("\r\nLine: ");
    serial_putdec(line);
    serial_puts("\r\nReason: ");
    serial_puts(reason);
    serial_puts("\r\n");

    /* Halt forever */
    for (;;) {
        hlt();
    }
}

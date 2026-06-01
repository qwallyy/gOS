/* gOS Kernel — printf Implementation
 * Minimal but functional printf family for kernel use.
 */

#include "lib/printf.h"
#include "lib/stdio.h"
#include "drivers/serial/serial.h"
#include "drivers/vga/vga.h"
#include <stdarg.h>

int kvsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    return vsnprintf(buf, size, fmt, args);
}

int ksnprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

int ksprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return ret;
}

int kvprintf(const char *fmt, va_list args) {
    char buf[1024];
    int ret = vsnprintf(buf, sizeof(buf), fmt, args);
    serial_puts(buf);
    vga_puts(buf);
    return ret;
}

int kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = kvprintf(fmt, args);
    va_end(args);
    return ret;
}

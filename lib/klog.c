/* gOS Kernel — Kernel Logging Implementation
 * Reference: https://wiki.osdev.org/Serial_Ports
 */

#include "gOS/klog.h"
#include "drivers/serial/serial.h"
#include "drivers/vga/vga.h"
#include "lib/string.h"
#include "lib/stdio.h"
#include <stdarg.h>

int g_klog_level = KLOG_DEBUG;

static const char *g_level_names[] = {
    "EMRG", "ALRT", "CRIT", "ERR ", "WARN",
    "NOTC", "INFO", "DBUG"
};

void klog_init(void) {
    g_klog_level = KLOG_DEBUG;
}

void klog(int level, const char *file, int line, const char *fmt, ...) {
    (void)file;
    (void)line;
    if (level > g_klog_level) return;

    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    /* Serial output */
    serial_puts("[");
    serial_puts(g_level_names[level]);
    serial_puts("] ");
    serial_puts(buf);
    serial_puts("\r\n");

    /* VGA output for NOTICE and above */
    if (level <= KLOG_NOTICE) {
        vga_puts("[");
        vga_puts(g_level_names[level]);
        vga_puts("] ");
        vga_puts(buf);
        vga_putchar('\n');
    }
}

void klog_raw(const char *str) {
    if (!str) return;
    serial_puts(str);
}

void klog_raw_char(char c) {
    serial_putc(c);
}

void klog_hexdump(const void *data, size_t len, uintptr_t base_addr) {
    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < len; i += 16) {
        char buf[128];
        int pos = 0;
        pos += snprintf(buf + pos, sizeof(buf) - pos, "%016llx  ",
                        (unsigned long long)(base_addr + i));
        for (size_t j = 0; j < 16; j++) {
            if (i + j < len) {
                pos += snprintf(buf + pos, sizeof(buf) - pos, "%02x ", bytes[i + j]);
            } else {
                pos += snprintf(buf + pos, sizeof(buf) - pos, "   ");
            }
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos, " |");
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            uint8_t c = bytes[i + j];
            pos += snprintf(buf + pos, sizeof(buf) - pos, "%c", (c >= 32 && c < 127) ? c : '.');
        }
        pos += snprintf(buf + pos, sizeof(buf) - pos, "|");
        klog_info("%s", buf);
    }
}

void klog_regdump(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx,
                  uint64_t rsi, uint64_t rdi, uint64_t rbp, uint64_t rsp,
                  uint64_t r8,  uint64_t r9,  uint64_t r10, uint64_t r11,
                  uint64_t r12, uint64_t r13, uint64_t r14, uint64_t r15,
                  uint64_t rip, uint64_t rflags, uint64_t cr2, uint64_t cr3) {
    klog_emerg("Register dump:");
    klog_emerg("  RAX=0x%016llx RBX=0x%016llx RCX=0x%016llx RDX=0x%016llx",
               (unsigned long long)rax, (unsigned long long)rbx,
               (unsigned long long)rcx, (unsigned long long)rdx);
    klog_emerg("  RSI=0x%016llx RDI=0x%016llx RBP=0x%016llx RSP=0x%016llx",
               (unsigned long long)rsi, (unsigned long long)rdi,
               (unsigned long long)rbp, (unsigned long long)rsp);
    klog_emerg("  R8 =0x%016llx R9 =0x%016llx R10=0x%016llx R11=0x%016llx",
               (unsigned long long)r8, (unsigned long long)r9,
               (unsigned long long)r10, (unsigned long long)r11);
    klog_emerg("  R12=0x%016llx R13=0x%016llx R14=0x%016llx R15=0x%016llx",
               (unsigned long long)r12, (unsigned long long)r13,
               (unsigned long long)r14, (unsigned long long)r15);
    klog_emerg("  RIP=0x%016llx RFLAGS=0x%016llx",
               (unsigned long long)rip, (unsigned long long)rflags);
    klog_emerg("  CR2=0x%016llx (fault addr) CR3=0x%016llx (page table)",
               (unsigned long long)cr2, (unsigned long long)cr3);
}

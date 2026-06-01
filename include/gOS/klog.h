/* gOS Kernel — Kernel Logging
 * Reference: https://wiki.osdev.org/Serial_Ports
 *
 * Unified kernel logging interface. All log output is directed to
 * COM1 serial port (0x3F8). The serial driver must be initialized
 * before klog() is usable.
 */

#ifndef GOS_KLOG_H
#define GOS_KLOG_H

#include "gOS/types.h"

/* Log levels */
#define KLOG_EMERG   0  /* System is unusable */
#define KLOG_ALERT   1  /* Action must be taken immediately */
#define KLOG_CRIT    2  /* Critical conditions */
#define KLOG_ERR     3  /* Error conditions */
#define KLOG_WARNING 4  /* Warning conditions */
#define KLOG_NOTICE  5  /* Normal but significant */
#define KLOG_INFO    6  /* Informational */
#define KLOG_DEBUG   7  /* Debug-level messages */

/* Current log level (can be overridden at runtime) */
extern int g_klog_level;

/* Initialize the kernel log (sets up serial) */
void klog_init(void);

/* Core log function — do not call directly, use macros below */
void klog(int level, const char *file, int line, const char *fmt, ...);

/* Convenience macros */
#define klog_emerg(...)   klog(KLOG_EMERG,   __FILE__, __LINE__, __VA_ARGS__)
#define klog_alert(...)   klog(KLOG_ALERT,   __FILE__, __LINE__, __VA_ARGS__)
#define klog_crit(...)    klog(KLOG_CRIT,    __FILE__, __LINE__, __VA_ARGS__)
#define klog_err(...)     klog(KLOG_ERR,     __FILE__, __LINE__, __VA_ARGS__)
#define klog_warn(...)    klog(KLOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define klog_notice(...)  klog(KLOG_NOTICE,  __FILE__, __LINE__, __VA_ARGS__)
#define klog_info(...)    klog(KLOG_INFO,    __FILE__, __LINE__, __VA_ARGS__)
#define klog_debug(...)   klog(KLOG_DEBUG,   __FILE__, __LINE__, __VA_ARGS__)

/* Raw serial output (for panic / early boot) */
void klog_raw(const char *str);
void klog_raw_char(char c);

/* Hex dump for debugging memory regions */
void klog_hexdump(const void *data, size_t len, uintptr_t base_addr);

/* Register dump for debugging crashes */
void klog_regdump(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx,
                  uint64_t rsi, uint64_t rdi, uint64_t rbp, uint64_t rsp,
                  uint64_t r8,  uint64_t r9,  uint64_t r10, uint64_t r11,
                  uint64_t r12, uint64_t r13, uint64_t r14, uint64_t r15,
                  uint64_t rip, uint64_t rflags, uint64_t cr2, uint64_t cr3);

#endif /* GOS_KLOG_H */

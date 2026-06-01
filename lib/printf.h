/* gOS Kernel — printf Family Declarations */

#ifndef GOS_PRINTF_H
#define GOS_PRINTF_H

#include "gOS/types.h"
#include "gOS/stdarg.h"

int kprintf(const char *fmt, ...);
int kvprintf(const char *fmt, va_list args);
int ksprintf(char *buf, size_t size, const char *fmt, ...);
int ksnprintf(char *buf, size_t size, const char *fmt, ...);
int kvsnprintf(char *buf, size_t size, const char *fmt, va_list args);

#endif /* GOS_PRINTF_H */

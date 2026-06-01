/* gOS Kernel — Standard I/O */

#ifndef GOS_STDIO_H
#define GOS_STDIO_H

#include "gOS/types.h"
#include "gOS/stdarg.h"

int snprintf(char *buf, size_t size, const char *fmt, ...);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

#endif /* GOS_STDIO_H */

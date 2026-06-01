/* gOS Kernel — Standard I/O Implementation */

#include "lib/stdio.h"
#include "lib/string.h"
#include <stdarg.h>

/* vsnprintf and snprintf are defined in lib/vsnprintf.c and lib/snprintf.c */

extern int __attribute__((used)) _stdio_stub(void);
int __attribute__((used)) _stdio_stub(void) { return 0; }

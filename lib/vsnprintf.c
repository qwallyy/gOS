/* gOS Kernel — vsnprintf Core Implementation
 * Supports %c, %s, %d, %i, %u, %x, %X, %p, %%, %lld, %llx, %llu, %ld, %lu
 */

#include "lib/stdio.h"
#include "lib/itoa.h"
#include "lib/string.h"

static int emit_char(char **buf, size_t *size, char c) {
    if (*size > 1) {
        **buf = c;
        (*buf)++;
        (*size)--;
        return 1;
    }
    return 0;
}

static int emit_string(char **buf, size_t *size, const char *str, size_t max) {
    int count = 0;
    for (size_t i = 0; str[i] && i < max; i++) {
        count += emit_char(buf, size, str[i]);
    }
    return count;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
    if (!buf || size == 0) return 0;
    char *out = buf;
    size_t remaining = size;
    int total = 0;

    for (const char *p = fmt; *p; p++) {
        if (*p != '%') {
            total += emit_char(&out, &remaining, *p);
            continue;
        }
        p++;
        bool long_long = false;
        bool long_mod = false;

        if (*p == 'l') {
            p++;
            if (*p == 'l') {
                long_long = true;
                p++;
            } else {
                long_mod = true;
            }
        }

        switch (*p) {
            case 'c': {
                char c = (char)va_arg(args, int);
                total += emit_char(&out, &remaining, c);
                break;
            }
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                total += emit_string(&out, &remaining, s, 4096);
                break;
            }
            case 'd':
            case 'i': {
                char tmp[32];
                if (long_long) {
                    lltoa(va_arg(args, long long), tmp, 10);
                } else if (long_mod) {
                    lltoa(va_arg(args, long), tmp, 10);
                } else {
                    itoa(va_arg(args, int), tmp, 10);
                }
                total += emit_string(&out, &remaining, tmp, 32);
                break;
            }
            case 'u': {
                char tmp[32];
                if (long_long) {
                    ulltoa(va_arg(args, unsigned long long), tmp, 10);
                } else if (long_mod) {
                    ulltoa(va_arg(args, unsigned long), tmp, 10);
                } else {
                    utoa(va_arg(args, unsigned int), tmp, 10);
                }
                total += emit_string(&out, &remaining, tmp, 32);
                break;
            }
            case 'x':
            case 'X': {
                char tmp[32];
                if (long_long) {
                    ulltoa(va_arg(args, unsigned long long), tmp, 16);
                } else if (long_mod) {
                    ulltoa(va_arg(args, unsigned long), tmp, 16);
                } else {
                    utoa(va_arg(args, unsigned int), tmp, 16);
                }
                total += emit_string(&out, &remaining, tmp, 32);
                break;
            }
            case 'p': {
                char tmp[32];
                utoa((unsigned int)(uintptr_t)va_arg(args, void *), tmp, 16);
                total += emit_string(&out, &remaining, "0x", 2);
                total += emit_string(&out, &remaining, tmp, 32);
                break;
            }
            case '%': {
                total += emit_char(&out, &remaining, '%');
                break;
            }
            default:
                total += emit_char(&out, &remaining, '%');
                total += emit_char(&out, &remaining, *p);
                break;
        }
    }

    *out = '\0';
    return total;
}

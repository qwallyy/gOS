/* gOS Kernel — vsnprintf Core Implementation
 * Supports flags (0, -), field width, length modifiers (l, ll, z, h, hh)
 * and conversions %c %s %d %i %u %x %X %p %%.
 * Examples: %016llx, %02x, %08x, %zu, %-10s, %3d.
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

/* Emit a value string, applying field width / padding.
 * - width: minimum field width
 * - left_align: pad on the right with spaces instead of left
 * - zero_pad: left-pad with '0' instead of ' ' (ignored when left_align)
 * A leading sign ('-') is kept in front of any zero padding. */
static int emit_padded(char **buf, size_t *size, const char *str, size_t len,
                       int width, bool left_align, bool zero_pad) {
    int count = 0;
    int pad = (int)width - (int)len;
    if (pad < 0) pad = 0;

    if (left_align) {
        for (size_t i = 0; i < len; i++) count += emit_char(buf, size, str[i]);
        for (int i = 0; i < pad; i++) count += emit_char(buf, size, ' ');
        return count;
    }

    if (zero_pad) {
        size_t i = 0;
        if (len > 0 && str[0] == '-') {        /* sign before the zeros */
            count += emit_char(buf, size, '-');
            i = 1;
        }
        for (int j = 0; j < pad; j++) count += emit_char(buf, size, '0');
        for (; i < len; i++) count += emit_char(buf, size, str[i]);
    } else {
        for (int j = 0; j < pad; j++) count += emit_char(buf, size, ' ');
        for (size_t i = 0; i < len; i++) count += emit_char(buf, size, str[i]);
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

        /* flags */
        bool left_align = false;
        bool zero_pad = false;
        for (;; p++) {
            if (*p == '-') left_align = true;
            else if (*p == '0') zero_pad = true;
            else break;
        }

        /* field width (digits) */
        int width = 0;
        while (*p >= '0' && *p <= '9') {
            width = width * 10 + (*p - '0');
            p++;
        }

        /* length modifier: l, ll, z (size_t), h, hh */
        bool long_long = false;   /* 64-bit */
        bool long_mod = false;    /* long == 64-bit on this LP64 kernel */
        if (*p == 'l') {
            p++;
            if (*p == 'l') { long_long = true; p++; }
            else long_mod = true;
        } else if (*p == 'z') {
            long_long = true;     /* size_t is 64-bit here */
            p++;
        } else if (*p == 'h') {
            p++;
            if (*p == 'h') p++;   /* hh: value still promoted to int */
        }

        char tmp[32];

        switch (*p) {
            case 'c': {
                char c = (char)va_arg(args, int);
                total += emit_padded(&out, &remaining, &c, 1, width, left_align, false);
                break;
            }
            case 's': {
                const char *s = va_arg(args, const char *);
                if (!s) s = "(null)";
                total += emit_padded(&out, &remaining, s, strlen(s),
                                     width, left_align, false);
                break;
            }
            case 'd':
            case 'i': {
                if (long_long) lltoa(va_arg(args, long long), tmp, 10);
                else if (long_mod) lltoa(va_arg(args, long), tmp, 10);
                else itoa(va_arg(args, int), tmp, 10);
                total += emit_padded(&out, &remaining, tmp, strlen(tmp),
                                     width, left_align, zero_pad);
                break;
            }
            case 'u': {
                if (long_long) ulltoa(va_arg(args, unsigned long long), tmp, 10);
                else if (long_mod) ulltoa(va_arg(args, unsigned long), tmp, 10);
                else utoa(va_arg(args, unsigned int), tmp, 10);
                total += emit_padded(&out, &remaining, tmp, strlen(tmp),
                                     width, left_align, zero_pad);
                break;
            }
            case 'x':
            case 'X': {
                if (long_long) ulltoa(va_arg(args, unsigned long long), tmp, 16);
                else if (long_mod) ulltoa(va_arg(args, unsigned long), tmp, 16);
                else utoa(va_arg(args, unsigned int), tmp, 16);
                total += emit_padded(&out, &remaining, tmp, strlen(tmp),
                                     width, left_align, zero_pad);
                break;
            }
            case 'p': {
                ulltoa((unsigned long long)(uintptr_t)va_arg(args, void *), tmp, 16);
                total += emit_char(&out, &remaining, '0');
                total += emit_char(&out, &remaining, 'x');
                total += emit_padded(&out, &remaining, tmp, strlen(tmp),
                                     width, left_align, zero_pad);
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

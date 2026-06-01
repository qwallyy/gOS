/* gOS Kernel — strtol Family Implementation */

#include "lib/strtol.h"
#include "lib/ctype.h"

static int digit_value(char c, int base) {
    int v = -1;
    if (c >= '0' && c <= '9') v = c - '0';
    else if (c >= 'a' && c <= 'z') v = c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z') v = c - 'A' + 10;
    return (v >= 0 && v < base) ? v : -1;
}

long strtol(const char *str, char **endptr, int base) {
    if (!str) return 0;
    while (isspace(*str)) str++;

    bool negative = false;
    if (*str == '-') { negative = true; str++; }
    else if (*str == '+') { str++; }

    if (base == 0) {
        if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
            base = 16;
            str += 2;
        } else if (str[0] == '0') {
            base = 8;
            str++;
        } else {
            base = 10;
        }
    } else if (base == 16 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    long result = 0;
    while (*str) {
        int digit = digit_value(*str, base);
        if (digit < 0) break;
        result = result * base + digit;
        str++;
    }

    if (endptr) *endptr = (char *)str;
    return negative ? -result : result;
}

long long strtoll(const char *str, char **endptr, int base) {
    return (long long)strtol(str, endptr, base);
}

unsigned long strtoul(const char *str, char **endptr, int base) {
    if (!str) return 0;
    while (isspace(*str)) str++;
    if (*str == '+') str++;

    if (base == 0) {
        if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
            base = 16;
            str += 2;
        } else if (str[0] == '0') {
            base = 8;
            str++;
        } else {
            base = 10;
        }
    }

    unsigned long result = 0;
    while (*str) {
        int digit = digit_value(*str, base);
        if (digit < 0) break;
        result = result * base + (unsigned long)digit;
        str++;
    }
    if (endptr) *endptr = (char *)str;
    return result;
}

unsigned long long strtoull(const char *str, char **endptr, int base) {
    return (unsigned long long)strtoul(str, endptr, base);
}

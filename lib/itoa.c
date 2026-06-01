/* gOS Kernel — Integer to String Implementation */

#include "lib/itoa.h"
#include "lib/string.h"

static void reverse(char *str, size_t len) {
    for (size_t i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

static char *int_to_str_internal(long long value, char *str, int base, bool is_unsigned) {
    if (base < 2 || base > 36) {
        str[0] = '\0';
        return str;
    }

    char *ptr = str;
    bool negative = false;

    if (!is_unsigned && value < 0) {
        negative = true;
        value = -value;
    }

    if (value == 0) {
        *ptr++ = '0';
    } else {
        while (value != 0) {
            int digit = (int)(value % base);
            *ptr++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
            value /= base;
        }
    }

    if (negative) *ptr++ = '-';
    *ptr = '\0';
    reverse(str, ptr - str);
    return str;
}

char *itoa(int value, char *str, int base) {
    return int_to_str_internal(value, str, base, false);
}

char *utoa(unsigned int value, char *str, int base) {
    return int_to_str_internal(value, str, base, true);
}

char *lltoa(long long value, char *str, int base) {
    return int_to_str_internal(value, str, base, false);
}

char *ulltoa(unsigned long long value, char *str, int base) {
    return int_to_str_internal(value, str, base, true);
}

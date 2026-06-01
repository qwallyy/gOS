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
    unsigned long long uval;

    if (!is_unsigned && value < 0) {
        negative = true;
        /* Negate via unsigned to stay well-defined for LLONG_MIN. */
        uval = (unsigned long long)(-(value + 1)) + 1ULL;
    } else {
        uval = (unsigned long long)value;
    }

    /* Digit extraction must use unsigned arithmetic: signed % / / on a value
     * whose top bit is set (e.g. a higher-half address reinterpreted as a
     * negative long long) yields negative digits and emits garbage bytes. */
    if (uval == 0) {
        *ptr++ = '0';
    } else {
        while (uval != 0) {
            unsigned int digit = (unsigned int)(uval % (unsigned long long)base);
            *ptr++ = (digit < 10) ? (char)('0' + digit)
                                  : (char)('a' + digit - 10);
            uval /= (unsigned long long)base;
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

/* gOS Kernel — Standard Library Implementation */

#include "lib/stdlib.h"

int atoi(const char *s) {
    int val = 0;
    bool neg = false;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '-') { neg = true; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return neg ? -val : val;
}

long atol(const char *s) {
    long val = 0;
    bool neg = false;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '-') { neg = true; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return neg ? -val : val;
}

int64_t atoll(const char *s) {
    int64_t val = 0;
    bool neg = false;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '-') { neg = true; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
    }
    return neg ? -val : val;
}

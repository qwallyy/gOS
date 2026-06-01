/* gOS Kernel — strspn/strcspn Implementation */

#include "lib/string.h"

static bool in_set(char c, const char *set) {
    for (const char *p = set; *p; p++) {
        if (*p == c) return true;
    }
    return false;
}

size_t strspn(const char *s, const char *accept) {
    size_t count = 0;
    for (const char *p = s; *p && in_set(*p, accept); p++) {
        count++;
    }
    return count;
}

size_t strcspn(const char *s, const char *reject) {
    size_t count = 0;
    for (const char *p = s; *p && !in_set(*p, reject); p++) {
        count++;
    }
    return count;
}

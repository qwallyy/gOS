/* gOS Kernel — strstr Implementation */

#include "lib/string.h"

char *strstr(const char *haystack, const char *needle) {
    if (!needle[0]) return (char *)haystack;
    if (!haystack) return NULL;

    size_t needle_len = strlen(needle);
    for (const char *p = haystack; *p; p++) {
        if (strncmp(p, needle, needle_len) == 0) {
            return (char *)p;
        }
    }
    return NULL;
}

/* gOS Kernel — strrchr Implementation */

#include "lib/string.h"

char *strrchr(const char *s, int c) {
    if (!s) return NULL;
    char *last = NULL;
    for (const char *p = s; *p; p++) {
        if (*p == (char)c) {
            last = (char *)p;
        }
    }
    if ((char)c == '\0') return (char *)s + strlen(s);
    return last;
}

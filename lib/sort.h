/* gOS Kernel — Sorting Utilities */

#ifndef GOS_SORT_H
#define GOS_SORT_H

#include "gOS/types.h"

void qsort(void *base, size_t nmemb, size_t size,
           int (*cmp)(const void *, const void *));
void heapsort(void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *));
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *));

#endif /* GOS_SORT_H */

/* gOS Kernel — Sorting Utilities Implementation */

#include "lib/sort.h"
#include "lib/string.h"

static void swap(void *a, void *b, size_t size) {
    uint8_t tmp;
    uint8_t *pa = a;
    uint8_t *pb = b;
    for (size_t i = 0; i < size; i++) {
        tmp = pa[i];
        pa[i] = pb[i];
        pb[i] = tmp;
    }
}

static int partition(void *base, size_t nmemb, size_t size,
                     int (*cmp)(const void *, const void *)) {
    uint8_t *arr = base;
    size_t pivot = nmemb / 2;
    size_t i = 0;
    size_t j = nmemb - 1;

    while (i <= j) {
        while (cmp(&arr[i * size], &arr[pivot * size]) < 0) i++;
        while (cmp(&arr[j * size], &arr[pivot * size]) > 0) j--;
        if (i <= j) {
            if (i != j) swap(&arr[i * size], &arr[j * size], size);
            i++;
            if (j > 0) j--;
        }
    }
    return (int)i;
}

void qsort(void *base, size_t nmemb, size_t size,
           int (*cmp)(const void *, const void *)) {
    if (nmemb <= 1) return;
    int p = partition(base, nmemb, size, cmp);
    if ((size_t)p > 1) {
        qsort(base, (size_t)p, size, cmp);
    }
    if ((size_t)p < nmemb - 1) {
        qsort((uint8_t *)base + p * size, nmemb - (size_t)p, size, cmp);
    }
}

static void sift_down(void *base, size_t start, size_t end, size_t size,
                      int (*cmp)(const void *, const void *)) {
    uint8_t *arr = base;
    size_t root = start;
    while (root * 2 + 1 <= end) {
        size_t child = root * 2 + 1;
        size_t swap_idx = root;
        if (cmp(&arr[swap_idx * size], &arr[child * size]) < 0) {
            swap_idx = child;
        }
        if (child + 1 <= end && cmp(&arr[swap_idx * size], &arr[(child + 1) * size]) < 0) {
            swap_idx = child + 1;
        }
        if (swap_idx == root) return;
        swap(&arr[root * size], &arr[swap_idx * size], size);
        root = swap_idx;
    }
}

void heapsort(void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *)) {
    if (nmemb <= 1) return;
    int count = (int)nmemb;
    int start = (count - 2) / 2;
    while (start >= 0) {
        sift_down(base, (size_t)start, nmemb - 1, size, cmp);
        start--;
    }
    size_t end = nmemb - 1;
    while (end > 0) {
        swap(base, (uint8_t *)base + end * size, size);
        end--;
        sift_down(base, 0, end, size, cmp);
    }
}

void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *)) {
    const uint8_t *arr = base;
    size_t lo = 0, hi = nmemb;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int c = cmp(key, &arr[mid * size]);
        if (c == 0) return (void *)&arr[mid * size];
        if (c < 0) hi = mid;
        else lo = mid + 1;
    }
    return NULL;
}

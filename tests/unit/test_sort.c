/* gOS Kernel — Sorting Unit Tests */

#include "lib/sort.h"
#include "lib/assert.h"

static int int_cmp(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

static void test_qsort(void) {
    int arr[] = {5, 2, 8, 1, 9, 3};
    qsort(arr, 6, sizeof(int), int_cmp);
    assert(arr[0] == 1);
    assert(arr[1] == 2);
    assert(arr[2] == 3);
    assert(arr[3] == 5);
    assert(arr[4] == 8);
    assert(arr[5] == 9);
}

static void test_heapsort(void) {
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6};
    heapsort(arr, 8, sizeof(int), int_cmp);
    assert(arr[0] == 1);
    assert(arr[1] == 1);
    assert(arr[2] == 2);
    assert(arr[3] == 3);
    assert(arr[4] == 4);
    assert(arr[5] == 5);
    assert(arr[6] == 6);
    assert(arr[7] == 9);
}

static void test_bsearch(void) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int key = 5;
    int *found = bsearch(&key, arr, 9, sizeof(int), int_cmp);
    assert(found != NULL);
    assert(*found == 5);

    int missing = 99;
    int *notfound = bsearch(&missing, arr, 9, sizeof(int), int_cmp);
    assert(notfound == NULL);
}

void run_sort_tests(void) {
    test_qsort();
    test_heapsort();
    test_bsearch();
}

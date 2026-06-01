/* gOS Kernel — Queue Unit Tests */

#include "lib/queue.h"
#include "lib/assert.h"
#include "mm/heap/heap.h"

static void test_queue_basic(void) {
    queue_t q;
    queue_init(&q);
    assert(queue_empty(&q));
    assert(queue_size(&q) == 0);

    int a = 1, b = 2, c = 3;
    assert(queue_enqueue(&q, &a));
    assert(queue_enqueue(&q, &b));
    assert(queue_enqueue(&q, &c));
    assert(queue_size(&q) == 3);

    int *r1 = queue_dequeue(&q);
    assert(r1 == &a);
    int *r2 = queue_dequeue(&q);
    assert(r2 == &b);
    int *r3 = queue_dequeue(&q);
    assert(r3 == &c);
    assert(queue_empty(&q));
}

static void test_queue_clear(void) {
    queue_t q;
    queue_init(&q);
    int x = 42;
    queue_enqueue(&q, &x);
    queue_enqueue(&q, &x);
    queue_clear(&q);
    assert(queue_empty(&q));
}

void run_queue_tests(void) {
    test_queue_basic();
    test_queue_clear();
}

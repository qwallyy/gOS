/* gOS Kernel — Ring Buffer Unit Tests */

#include "gOS/ring_buffer.h"
#include "lib/assert.h"

static void test_ring_buffer_basic(void) {
    uint8_t buf[16];
    ring_buffer_t rb;
    ring_buffer_init(&rb, buf, sizeof(buf));

    assert(ring_buffer_empty(&rb));
    assert(!ring_buffer_full(&rb));

    assert(ring_buffer_push(&rb, 0xAA));
    assert(ring_buffer_push(&rb, 0xBB));
    assert(!ring_buffer_empty(&rb));
    assert(ring_buffer_size(&rb) == 2);

    uint8_t b;
    assert(ring_buffer_pop(&rb, &b) && b == 0xAA);
    assert(ring_buffer_pop(&rb, &b) && b == 0xBB);
    assert(ring_buffer_empty(&rb));
}

static void test_ring_buffer_wrap(void) {
    uint8_t buf[4];
    ring_buffer_t rb;
    ring_buffer_init(&rb, buf, sizeof(buf));

    assert(ring_buffer_push(&rb, 1));
    assert(ring_buffer_push(&rb, 2));
    assert(ring_buffer_push(&rb, 3));
    /* Full at capacity-1 for this simple ring */

    uint8_t b;
    assert(ring_buffer_pop(&rb, &b) && b == 1);
    assert(ring_buffer_push(&rb, 4));
    assert(ring_buffer_pop(&rb, &b) && b == 2);
    assert(ring_buffer_pop(&rb, &b) && b == 3);
    assert(ring_buffer_pop(&rb, &b) && b == 4);
    assert(ring_buffer_empty(&rb));
}

void run_ring_buffer_tests(void) {
    test_ring_buffer_basic();
    test_ring_buffer_wrap();
}

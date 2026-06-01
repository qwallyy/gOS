/* gOS Kernel — Ring Buffer (Circular Queue)
 * Reference: Producer-consumer buffer pattern
 */

#ifndef GOS_RING_BUFFER_H
#define GOS_RING_BUFFER_H

#include "gOS/types.h"

typedef struct ring_buffer {
    uint8_t *buffer;
    size_t capacity;
    volatile size_t head;
    volatile size_t tail;
} ring_buffer_t;

static inline void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, size_t cap) {
    rb->buffer = buf;
    rb->capacity = cap;
    rb->head = 0;
    rb->tail = 0;
}

static inline bool ring_buffer_empty(const ring_buffer_t *rb) {
    return rb->head == rb->tail;
}

static inline bool ring_buffer_full(const ring_buffer_t *rb) {
    return ((rb->head + 1) % rb->capacity) == rb->tail;
}

static inline size_t ring_buffer_size(const ring_buffer_t *rb) {
    return (rb->head >= rb->tail)
        ? (rb->head - rb->tail)
        : (rb->capacity - rb->tail + rb->head);
}

static inline bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte) {
    if (ring_buffer_full(rb)) return false;
    rb->buffer[rb->head] = byte;
    rb->head = (rb->head + 1) % rb->capacity;
    return true;
}

static inline bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *byte) {
    if (ring_buffer_empty(rb)) return false;
    *byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    return true;
}

static inline void ring_buffer_clear(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
}

#endif /* GOS_RING_BUFFER_H */

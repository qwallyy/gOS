/* gOS Kernel — Generic Queue Implementation */

#include "lib/queue.h"
#include "mm/heap/heap.h"

void queue_init(queue_t *q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

bool queue_enqueue(queue_t *q, void *data) {
    queue_node_t *node = kmalloc(sizeof(queue_node_t));
    if (!node) return false;
    node->data = data;
    node->next = NULL;
    if (q->tail) {
        q->tail->next = node;
    } else {
        q->head = node;
    }
    q->tail = node;
    q->count++;
    return true;
}

void *queue_dequeue(queue_t *q) {
    if (!q->head) return NULL;
    queue_node_t *node = q->head;
    void *data = node->data;
    q->head = node->next;
    if (!q->head) q->tail = NULL;
    kfree(node);
    q->count--;
    return data;
}

bool queue_empty(const queue_t *q) {
    return q->count == 0;
}

size_t queue_size(const queue_t *q) {
    return q->count;
}

void queue_clear(queue_t *q) {
    while (!queue_empty(q)) {
        queue_dequeue(q);
    }
}

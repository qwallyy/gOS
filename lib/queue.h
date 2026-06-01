/* gOS Kernel — Generic Queue (FIFO)
 * Reference: Standard queue pattern using kmalloc
 */

#ifndef GOS_QUEUE_H
#define GOS_QUEUE_H

#include "gOS/types.h"

typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node_t;

typedef struct queue {
    queue_node_t *head;
    queue_node_t *tail;
    size_t count;
} queue_t;

void queue_init(queue_t *q);
bool queue_enqueue(queue_t *q, void *data);
void *queue_dequeue(queue_t *q);
bool queue_empty(const queue_t *q);
size_t queue_size(const queue_t *q);
void queue_clear(queue_t *q);

#endif /* GOS_QUEUE_H */

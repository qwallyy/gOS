/* gOS Kernel — Intrusive Linked List
 * Reference: Linux kernel list.h pattern
 */

#ifndef GOS_LIST_H
#define GOS_LIST_H

#include "gOS/types.h"

/* Intrusive doubly-linked list node */
typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

/* Static initializer */
#define LIST_NODE_INIT(name) { &(name), &(name) }

static inline void list_init(list_node_t *node) {
    node->next = node;
    node->prev = node;
}

static inline bool list_empty(const list_node_t *head) {
    return head->next == head;
}

static inline void list_insert_after(list_node_t *pos, list_node_t *new_node) {
    new_node->next = pos->next;
    new_node->prev = pos;
    pos->next->prev = new_node;
    pos->next = new_node;
}

static inline void list_insert_before(list_node_t *pos, list_node_t *new_node) {
    new_node->next = pos;
    new_node->prev = pos->prev;
    pos->prev->next = new_node;
    pos->prev = new_node;
}

static inline void list_append(list_node_t *head, list_node_t *new_node) {
    list_insert_before(head, new_node);
}

static inline void list_prepend(list_node_t *head, list_node_t *new_node) {
    list_insert_after(head, new_node);
}

static inline void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}

static inline void list_replace(list_node_t *old_node, list_node_t *new_node) {
    new_node->next = old_node->next;
    new_node->prev = old_node->prev;
    old_node->prev->next = new_node;
    old_node->next->prev = new_node;
}

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/* Get containing structure from list node */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - __builtin_offsetof(type, member)))

#define list_first_entry(head, type, member) \
    list_entry((head)->next, type, member)

#define list_last_entry(head, type, member) \
    list_entry((head)->prev, type, member)

#endif /* GOS_LIST_H */

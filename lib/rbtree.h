/* gOS Kernel — Red-Black Tree
 * Reference: Introduction to Algorithms (CLRS)
 */

#ifndef GOS_RBTREE_H
#define GOS_RBTREE_H

#include "gOS/types.h"

#define RBTREE_RED      0
#define RBTREE_BLACK    1

typedef struct rb_node {
    struct rb_node *parent;
    struct rb_node *left;
    struct rb_node *right;
    uint8_t color;
} rb_node_t;

typedef struct rb_tree {
    rb_node_t *root;
    size_t count;
} rb_tree_t;

typedef int (*rb_compare_fn)(const rb_node_t *a, const rb_node_t *b);

void rb_tree_init(rb_tree_t *tree);
void rb_insert(rb_tree_t *tree, rb_node_t *node, rb_compare_fn cmp);
void rb_remove(rb_tree_t *tree, rb_node_t *node);
rb_node_t *rb_search(rb_tree_t *tree, rb_node_t *key, rb_compare_fn cmp);
rb_node_t *rb_min(rb_tree_t *tree);
rb_node_t *rb_max(rb_tree_t *tree);
rb_node_t *rb_next(rb_node_t *node);
rb_node_t *rb_prev(rb_node_t *node);

#define rb_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - __builtin_offsetof(type, member)))

#endif /* GOS_RBTREE_H */

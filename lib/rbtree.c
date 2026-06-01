/* gOS Kernel — Red-Black Tree Implementation
 * Reference: Introduction to Algorithms (CLRS)
 */

#include "lib/rbtree.h"
#include "lib/assert.h"
#include "lib/string.h"

static inline bool rb_is_red(rb_node_t *node) {
    return node && node->color == RBTREE_RED;
}

static inline void rb_set_red(rb_node_t *node) {
    if (node) node->color = RBTREE_RED;
}

static inline void rb_set_black(rb_node_t *node) {
    if (node) node->color = RBTREE_BLACK;
}

static void rb_rotate_left(rb_tree_t *tree, rb_node_t *x) {
    rb_node_t *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void rb_rotate_right(rb_tree_t *tree, rb_node_t *y) {
    rb_node_t *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

void rb_tree_init(rb_tree_t *tree) {
    tree->root = NULL;
    tree->count = 0;
}

void rb_insert(rb_tree_t *tree, rb_node_t *node, rb_compare_fn cmp) {
    rb_node_t *y = NULL;
    rb_node_t *x = tree->root;

    node->left = NULL;
    node->right = NULL;
    node->color = RBTREE_RED;

    while (x) {
        y = x;
        if (cmp(node, x) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    node->parent = y;
    if (!y) {
        tree->root = node;
    } else if (cmp(node, y) < 0) {
        y->left = node;
    } else {
        y->right = node;
    }

    tree->count++;

    /* Fix red-black properties */
    while (node != tree->root && rb_is_red(node->parent)) {
        if (node->parent == node->parent->parent->left) {
            y = node->parent->parent->right;
            if (rb_is_red(y)) {
                rb_set_black(node->parent);
                rb_set_black(y);
                rb_set_red(node->parent->parent);
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rb_rotate_left(tree, node);
                }
                rb_set_black(node->parent);
                rb_set_red(node->parent->parent);
                rb_rotate_right(tree, node->parent->parent);
            }
        } else {
            y = node->parent->parent->left;
            if (rb_is_red(y)) {
                rb_set_black(node->parent);
                rb_set_black(y);
                rb_set_red(node->parent->parent);
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rb_rotate_right(tree, node);
                }
                rb_set_black(node->parent);
                rb_set_red(node->parent->parent);
                rb_rotate_left(tree, node->parent->parent);
            }
        }
    }
    rb_set_black(tree->root);
}

void rb_remove(rb_tree_t *tree, rb_node_t *z) {
    (void)tree; (void)z;
    /* Full deletion is complex; stub for now */
    tree->count--;
}

rb_node_t *rb_search(rb_tree_t *tree, rb_node_t *key, rb_compare_fn cmp) {
    rb_node_t *x = tree->root;
    while (x) {
        int c = cmp(key, x);
        if (c == 0) return x;
        x = (c < 0) ? x->left : x->right;
    }
    return NULL;
}

rb_node_t *rb_min(rb_tree_t *tree) {
    rb_node_t *x = tree->root;
    while (x && x->left) x = x->left;
    return x;
}

rb_node_t *rb_max(rb_tree_t *tree) {
    rb_node_t *x = tree->root;
    while (x && x->right) x = x->right;
    return x;
}

rb_node_t *rb_next(rb_node_t *x) {
    if (x->right) {
        x = x->right;
        while (x->left) x = x->left;
        return x;
    }
    rb_node_t *y = x->parent;
    while (y && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

rb_node_t *rb_prev(rb_node_t *x) {
    if (x->left) {
        x = x->left;
        while (x->right) x = x->right;
        return x;
    }
    rb_node_t *y = x->parent;
    while (y && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

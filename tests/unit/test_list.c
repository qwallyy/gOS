/* gOS Kernel — Intrusive List Unit Tests */

#include "gOS/list.h"
#include "lib/assert.h"

static void test_list_init(void) {
    list_node_t head;
    list_init(&head);
    assert(list_empty(&head));
}

static void test_list_insert_remove(void) {
    list_node_t head;
    list_init(&head);

    list_node_t n1, n2, n3;
    list_init(&n1);
    list_init(&n2);
    list_init(&n3);

    list_append(&head, &n1);
    list_append(&head, &n2);
    list_append(&head, &n3);
    assert(!list_empty(&head));

    list_remove(&n2);
    list_remove(&n1);
    list_remove(&n3);
    assert(list_empty(&head));
}

static void test_list_for_each(void) {
    list_node_t head;
    list_init(&head);

    list_node_t n1, n2;
    list_init(&n1);
    list_init(&n2);

    list_append(&head, &n1);
    list_append(&head, &n2);

    int count = 0;
    list_node_t *pos;
    list_for_each(pos, &head) {
        count++;
    }
    assert(count == 2);
}

void run_list_tests(void) {
    test_list_init();
    test_list_insert_remove();
    test_list_for_each();
}

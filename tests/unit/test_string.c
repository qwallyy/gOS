/* gOS Kernel — String Utility Unit Tests */

#include "lib/string.h"
#include "lib/assert.h"

static void test_strlen(void) {
    assert(strlen("") == 0);
    assert(strlen("hello") == 5);
    assert(strlen("a") == 1);
}

static void test_strcmp(void) {
    assert(strcmp("abc", "abc") == 0);
    assert(strcmp("abc", "abd") < 0);
    assert(strcmp("abd", "abc") > 0);
    assert(strcmp("ab", "abc") < 0);
}

static void test_strncmp(void) {
    assert(strncmp("abc", "abc", 3) == 0);
    assert(strncmp("abc", "abd", 2) == 0);
    assert(strncmp("abc", "abd", 3) < 0);
}

static void test_strcpy(void) {
    char buf[32];
    strcpy(buf, "test");
    assert(strcmp(buf, "test") == 0);
}

static void test_strncpy(void) {
    char buf[32] = {0};
    strncpy(buf, "hello", 3);
    assert(buf[0] == 'h' && buf[1] == 'e' && buf[2] == 'l');
    assert(buf[3] == '\0');
}

static void test_strcat(void) {
    char buf[32] = "hello";
    strcat(buf, " world");
    assert(strcmp(buf, "hello world") == 0);
}

static void test_strchr(void) {
    assert(strchr("hello", 'l') != NULL);
    assert(strchr("hello", 'z') == NULL);
}

static void test_memcpy(void) {
    char src[] = "abcdef";
    char dst[32];
    memcpy(dst, src, 7);
    assert(strcmp(dst, "abcdef") == 0);
}

static void test_memset(void) {
    char buf[32];
    memset(buf, 'A', 10);
    for (int i = 0; i < 10; i++) assert(buf[i] == 'A');
}

static void test_memcmp(void) {
    char a[] = "abc";
    char b[] = "abc";
    char c[] = "abd";
    assert(memcmp(a, b, 3) == 0);
    assert(memcmp(a, c, 3) < 0);
}

void run_string_tests(void) {
    test_strlen();
    test_strcmp();
    test_strncmp();
    test_strcpy();
    test_strncpy();
    test_strcat();
    test_strchr();
    test_memcpy();
    test_memset();
    test_memcmp();
}

/* gOS Kernel — CRC32 Unit Tests */

#include "lib/crc32.h"
#include "lib/assert.h"
#include "lib/string.h"

static void test_crc32_empty(void) {
    uint32_t crc = crc32("", 0);
    assert(crc == 0x00000000); /* Actually CRC32 of empty is 0 */
}

static void test_crc32_known(void) {
    /* CRC32 of "123456789" is 0xCBF43926 */
    uint32_t crc = crc32("123456789", 9);
    assert(crc == 0xCBF43926);
}

static void test_crc32_update(void) {
    uint32_t crc = crc32_update(0, "hello", 5);
    uint32_t crc2 = crc32("hello", 5);
    assert(crc == crc2);
}

static void test_crc32_ones(void) {
    uint32_t crc = crc32("\xFF\xFF\xFF\xFF", 4);
    /* Just verify it produces a consistent result */
    uint32_t crc2 = crc32("\xFF\xFF\xFF\xFF", 4);
    assert(crc == crc2);
}

void run_crc32_tests(void) {
    test_crc32_empty();
    test_crc32_known();
    test_crc32_update();
    test_crc32_ones();
}

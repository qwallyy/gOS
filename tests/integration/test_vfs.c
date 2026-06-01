/* gOS Kernel — VFS Integration Tests */

#include "fs/vfs/vfs.h"
#include "fs/devfs/devfs.h"
#include "fs/tmpfs/tmpfs.h"
#include "gOS/klog.h"
#include "lib/assert.h"
#include "lib/string.h"

static void test_vfs_init(void) {
    vfs_init();
    klog_info("[VFS_TEST] VFS initialized");
}

static void test_devfs_mount(void) {
    devfs_init();
    vfs_ops_t *ops = NULL;
    bool ok = devfs_mount(&ops);
    assert(ok);
    assert(ops != NULL);
    klog_info("[VFS_TEST] DEVFS mounted");
}

static void test_tmpfs_mount(void) {
    tmpfs_init();
    vfs_ops_t *ops = NULL;
    bool ok = tmpfs_mount(&ops);
    assert(ok);
    assert(ops != NULL);
    klog_info("[VFS_TEST] TMPFS mounted");
}

static void test_vfs_file_ops(void) {
    vfs_node_t *null_dev = vfs_lookup(NULL, "null");
    if (null_dev) {
        char buf[16];
        int r = vfs_read(null_dev, 0, buf, 16);
        assert(r == 0);
        int w = vfs_write(null_dev, 0, "hello", 5);
        assert(w == 5);
        klog_info("[VFS_TEST] /dev/null operations OK");
    }
}

void run_vfs_integration_tests(void) {
    klog_info("================================");
    klog_info("  VFS Integration Tests");
    klog_info("================================");

    test_vfs_init();
    test_devfs_mount();
    test_tmpfs_mount();
    test_vfs_file_ops();

    klog_info("================================");
    klog_info("  VFS Tests PASSED");
    klog_info("================================");
}

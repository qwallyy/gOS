/* gOS Kernel — Device Filesystem Implementation
 * Provides /dev/null, /dev/zero, /dev/random, /dev/console
 */

#include "fs/devfs/devfs.h"
#include "mm/heap/heap.h"
#include "drivers/serial/serial.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

static devfs_device_t g_devfs_devices[DEVFS_MAX_DEVICES];

static int dev_null_read(void *buf, size_t count, uint64_t offset) {
    (void)buf; (void)count; (void)offset;
    return 0;
}

static int dev_null_write(const void *buf, size_t count, uint64_t offset) {
    (void)buf; (void)offset;
    return (int)count;
}

static int dev_zero_read(void *buf, size_t count, uint64_t offset) {
    (void)offset;
    if (!buf) return 0;
    memset(buf, 0, count);
    return (int)count;
}

static int dev_zero_write(const void *buf, size_t count, uint64_t offset) {
    (void)buf; (void)count; (void)offset;
    return -EIO;
}

static int dev_random_read(void *buf, size_t count, uint64_t offset) {
    (void)offset;
    if (!buf) return 0;
    /* Simple LCG PRNG for stub */
    static uint32_t seed = 12345;
    uint8_t *out = buf;
    for (size_t i = 0; i < count; i++) {
        seed = seed * 1103515245 + 12345;
        out[i] = (uint8_t)(seed >> 16);
    }
    return (int)count;
}

static int dev_console_read(void *buf, size_t count, uint64_t offset) {
    (void)offset;
    if (!buf || count == 0) return 0;
    /* Read from serial */
    char *out = buf;
    out[0] = serial_getc();
    return 1;
}

static int dev_console_write(const void *buf, size_t count, uint64_t offset) {
    (void)offset;
    if (!buf) return 0;
    const char *str = buf;
    for (size_t i = 0; i < count; i++) {
        serial_putc(str[i]);
    }
    return (int)count;
}

void devfs_init(void) {
    klog_info("DEVFS initialized");
    for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
        g_devfs_devices[i].valid = false;
    }

    devfs_register("null", DEV_TYPE_CHAR, dev_null_read, dev_null_write);
    devfs_register("zero", DEV_TYPE_CHAR, dev_zero_read, dev_zero_write);
    devfs_register("random", DEV_TYPE_CHAR, dev_random_read, NULL);
    devfs_register("console", DEV_TYPE_CHAR, dev_console_read, dev_console_write);
}

int devfs_register(const char *name, uint32_t type,
                   int (*read)(void *, size_t, uint64_t),
                   int (*write)(const void *, size_t, uint64_t)) {
    for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
        if (!g_devfs_devices[i].valid) {
            strncpy(g_devfs_devices[i].name, name, 31);
            g_devfs_devices[i].name[31] = '\0';
            g_devfs_devices[i].type = type;
            g_devfs_devices[i].read = read;
            g_devfs_devices[i].write = write;
            g_devfs_devices[i].valid = true;
            klog_info("DEVFS: registered /dev/%s", name);
            return 0;
        }
    }
    return -ENOSPC;
}

static vfs_node_t *devfs_vfs_lookup(vfs_node_t *parent, const char *name) {
    (void)parent;
    for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
        if (g_devfs_devices[i].valid && strcmp(g_devfs_devices[i].name, name) == 0) {
            vfs_node_t *vn = kzalloc(sizeof(vfs_node_t));
            if (!vn) return NULL;
            strncpy(vn->name, name, VFS_NAME_MAX - 1);
            vn->type = (g_devfs_devices[i].type == DEV_TYPE_BLOCK)
                        ? VFS_NODE_DEVICE : VFS_NODE_DEVICE;
            vn->private_data = &g_devfs_devices[i];
            return vn;
        }
    }
    return NULL;
}

static int devfs_vfs_read(vfs_node_t *node, uint64_t offset, void *buf, size_t count) {
    if (!node || !node->private_data) return -EINVAL;
    devfs_device_t *dev = node->private_data;
    if (!dev->read) return -EIO;
    return dev->read(buf, count, offset);
}

static int devfs_vfs_write(vfs_node_t *node, uint64_t offset, const void *buf, size_t count) {
    if (!node || !node->private_data) return -EINVAL;
    devfs_device_t *dev = node->private_data;
    if (!dev->write) return -EIO;
    return dev->write(buf, count, offset);
}

static vfs_ops_t g_devfs_ops = {
    .lookup = devfs_vfs_lookup,
    .read = devfs_vfs_read,
    .write = devfs_vfs_write,
    .readdir = NULL,
    .create = NULL,
    .mkdir = NULL,
    .remove = NULL,
};

bool devfs_mount(vfs_ops_t **ops_out) {
    klog_info("DEVFS mounted at /dev");
    if (ops_out) *ops_out = &g_devfs_ops;
    return true;
}

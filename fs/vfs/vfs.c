/* gOS Kernel — Virtual File System Implementation
 * Reference: https://wiki.osdev.org/VFS
 */

#include "fs/vfs/vfs.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "lib/string.h"

static vfs_mount_t g_mounts[VFS_MAX_MOUNTS];
static vfs_file_t g_open_files[VFS_MAX_OPEN];
static int __attribute__((used)) g_next_fd = 3; /* 0=stdin, 1=stdout, 2=stderr */

static vfs_node_t * __attribute__((used)) vfs_alloc_node(void) {
    vfs_node_t *node = kzalloc(sizeof(vfs_node_t));
    if (node) node->refcount = 1;
    return node;
}

static void vfs_free_node(vfs_node_t *node) {
    if (!node) return;
    node->refcount--;
    if (node->refcount == 0) {
        kfree(node);
    }
}

void vfs_init(void) {
    klog_info("Initializing VFS");
    for (int i = 0; i < VFS_MAX_MOUNTS; i++) {
        g_mounts[i].root = NULL;
        g_mounts[i].ops = NULL;
    }
    for (int i = 0; i < VFS_MAX_OPEN; i++) {
        g_open_files[i].fd = -1;
    }
    klog_info("VFS initialized");
}

void vfs_register_root(vfs_ops_t *ops, void *private) {
    if (!ops) return;
    g_mounts[0].ops = ops;
    g_mounts[0].private_data = private;
    strcpy(g_mounts[0].mountpoint, "/");
    klog_info("VFS root registered");
}

int vfs_mount(const char *path, vfs_ops_t *ops, void *private) {
    for (int i = 1; i < VFS_MAX_MOUNTS; i++) {
        if (g_mounts[i].ops == NULL) {
            g_mounts[i].ops = ops;
            g_mounts[i].private_data = private;
            strncpy(g_mounts[i].mountpoint, path, VFS_PATH_MAX - 1);
            klog_info("VFS mounted '%s'", path);
            return 0;
        }
    }
    klog_err("VFS: no free mount slots");
    return -1;
}

int vfs_open(const char *path, uint32_t flags) {
    (void)path; (void)flags;
    klog_warn("vfs_open: no filesystem mounted yet");
    return -1;
}

int vfs_close(int fd) {
    if (fd < 0 || fd >= VFS_MAX_OPEN) return -1;
    if (g_open_files[fd].fd < 0) return -1;
    g_open_files[fd].fd = -1;
    if (g_open_files[fd].node) {
        vfs_free_node(g_open_files[fd].node);
        g_open_files[fd].node = NULL;
    }
    return 0;
}

int vfs_read(int fd, void *buf, size_t count) {
    if (fd < 0 || fd >= VFS_MAX_OPEN) return -1;
    if (g_open_files[fd].fd < 0) return -1;
    vfs_file_t *f = &g_open_files[fd];
    if (!f->node || !f->node->ops || !f->node->ops->read) return -1;
    int ret = f->node->ops->read(f->node, f->offset, buf, count);
    if (ret > 0) f->offset += ret;
    return ret;
}

int vfs_write(int fd, const void *buf, size_t count) {
    if (fd < 0 || fd >= VFS_MAX_OPEN) return -1;
    if (g_open_files[fd].fd < 0) return -1;
    vfs_file_t *f = &g_open_files[fd];
    if (!f->node || !f->node->ops || !f->node->ops->write) return -1;
    int ret = f->node->ops->write(f->node, f->offset, buf, count);
    if (ret > 0) f->offset += ret;
    return ret;
}

int vfs_seek(int fd, int64_t offset, int whence) {
    if (fd < 0 || fd >= VFS_MAX_OPEN) return -1;
    if (g_open_files[fd].fd < 0) return -1;
    vfs_file_t *f = &g_open_files[fd];
    switch (whence) {
        case SEEK_SET: f->offset = offset; break;
        case SEEK_CUR: f->offset += offset; break;
        case SEEK_END:
            if (f->node) f->offset = f->node->size + offset;
            break;
        default: return -1;
    }
    return (int)f->offset;
}

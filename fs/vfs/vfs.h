/* gOS Kernel — Virtual File System (VFS)
 * Reference: https://wiki.osdev.org/VFS
 */

#ifndef GOS_VFS_H
#define GOS_VFS_H

#include "gOS/types.h"

#define VFS_NAME_MAX        256
#define VFS_PATH_MAX        4096
#define VFS_MAX_MOUNTS      16
#define VFS_MAX_OPEN        256

#define O_RDONLY    0x0000
#define O_WRONLY    0x0001
#define O_RDWR      0x0002
#define O_CREAT     0x0040
#define O_APPEND    0x0400
#define O_TRUNC     0x0200

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

typedef struct vfs_node vfs_node_t;
typedef struct vfs_mount vfs_mount_t;
typedef struct vfs_file vfs_file_t;

/* Node types */
#define VFS_NODE_FILE       1
#define VFS_NODE_DIRECTORY  2
#define VFS_NODE_SYMLINK    3
#define VFS_NODE_DEVICE     4
#define VFS_NODE_PIPE       5

/* VFS operations */
typedef struct vfs_ops {
    vfs_node_t *(*lookup)(vfs_node_t *parent, const char *name);
    int (*read)(vfs_node_t *node, uint64_t offset, void *buf, size_t count);
    int (*write)(vfs_node_t *node, uint64_t offset, const void *buf, size_t count);
    int (*readdir)(vfs_node_t *node, uint64_t offset, void *buf, size_t count);
    int (*create)(vfs_node_t *parent, const char *name, uint32_t mode);
    int (*mkdir)(vfs_node_t *parent, const char *name, uint32_t mode);
    int (*remove)(vfs_node_t *parent, const char *name);
} vfs_ops_t;

struct vfs_node {
    char name[VFS_NAME_MAX];
    uint32_t type;
    uint64_t size;
    uint32_t permissions;
    uint32_t uid;
    uint32_t gid;
    uint64_t ctime;
    uint64_t mtime;
    uint32_t refcount;
    vfs_mount_t *mount;
    vfs_ops_t *ops;
    void *private_data;
    vfs_node_t *parent;
    vfs_node_t *next_sibling;
    vfs_node_t *first_child;
};

struct vfs_mount {
    char mountpoint[VFS_PATH_MAX];
    vfs_node_t *root;
    vfs_ops_t *ops;
    void *private_data;
};

struct vfs_file {
    int fd;
    uint32_t flags;
    uint64_t offset;
    vfs_node_t *node;
    int refcount;
};

void vfs_init(void);
int vfs_mount(const char *path, vfs_ops_t *ops, void *private);
int vfs_open(const char *path, uint32_t flags);
int vfs_close(int fd);
int vfs_read(int fd, void *buf, size_t count);
int vfs_write(int fd, const void *buf, size_t count);
int vfs_seek(int fd, int64_t offset, int whence);
void vfs_register_root(vfs_ops_t *ops, void *private);

#endif /* GOS_VFS_H */

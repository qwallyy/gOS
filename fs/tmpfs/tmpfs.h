/* gOS Kernel — TMPFS (In-Memory Filesystem)
 * Reference: https://wiki.osdev.org/TMPFS
 */

#ifndef GOS_TMPFS_H
#define GOS_TMPFS_H

#include "gOS/types.h"
#include "fs/vfs/vfs.h"

#define TMPFS_MAX_FILES     64
#define TMPFS_MAX_SIZE      (256 * 1024)  /* 256KB total */

typedef struct tmpfs_node {
    char name[VFS_NAME_MAX];
    uint32_t type;
    uint8_t *data;
    size_t size;
    size_t capacity;
    bool valid;
} tmpfs_node_t;

void tmpfs_init(void);
bool tmpfs_mount(vfs_ops_t **ops_out);

#endif /* GOS_TMPFS_H */

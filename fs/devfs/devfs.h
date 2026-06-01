/* gOS Kernel — Device Filesystem
 * Provides /dev/null, /dev/zero, /dev/random, /dev/console
 */

#ifndef GOS_DEVFS_H
#define GOS_DEVFS_H

#include "gOS/types.h"
#include "fs/vfs/vfs.h"

#define DEVFS_MAX_DEVICES   32

typedef struct devfs_device {
    char name[32];
    uint32_t type;
    int (*read)(void *buf, size_t count, uint64_t offset);
    int (*write)(const void *buf, size_t count, uint64_t offset);
    bool valid;
} devfs_device_t;

#define DEV_TYPE_CHAR       1
#define DEV_TYPE_BLOCK      2

void devfs_init(void);
bool devfs_mount(vfs_ops_t **ops_out);
int  devfs_register(const char *name, uint32_t type,
                    int (*read)(void *, size_t, uint64_t),
                    int (*write)(const void *, size_t, uint64_t));

#endif /* GOS_DEVFS_H */

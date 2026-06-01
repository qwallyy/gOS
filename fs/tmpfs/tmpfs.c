/* gOS Kernel — TMPFS Implementation
 * Reference: https://wiki.osdev.org/TMPFS
 */

#include "fs/tmpfs/tmpfs.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

static tmpfs_node_t g_tmpfs_nodes[TMPFS_MAX_FILES];
static size_t g_tmpfs_used = 0;

static tmpfs_node_t *tmpfs_find(const char *name) {
    for (int i = 0; i < TMPFS_MAX_FILES; i++) {
        if (g_tmpfs_nodes[i].valid && strcmp(g_tmpfs_nodes[i].name, name) == 0) {
            return &g_tmpfs_nodes[i];
        }
    }
    return NULL;
}
 __attribute__((used)) 
static tmpfs_node_t *tmpfs_create_node(const char *name, uint32_t type) {
    if (g_tmpfs_used >= TMPFS_MAX_FILES) return NULL;
    for (int i = 0; i < TMPFS_MAX_FILES; i++) {
        if (!g_tmpfs_nodes[i].valid) {
            strncpy(g_tmpfs_nodes[i].name, name, VFS_NAME_MAX - 1);
            g_tmpfs_nodes[i].name[VFS_NAME_MAX - 1] = '\0';
            g_tmpfs_nodes[i].type = type;
            g_tmpfs_nodes[i].data = NULL;
            g_tmpfs_nodes[i].size = 0;
            g_tmpfs_nodes[i].capacity = 0;
            g_tmpfs_nodes[i].valid = true;
            g_tmpfs_used++;
            return &g_tmpfs_nodes[i];
        }
    }
    return NULL;
}

static vfs_node_t *tmpfs_vfs_lookup(vfs_node_t *parent, const char *name) {
    (void)parent;
    tmpfs_node_t *node = tmpfs_find(name);
    if (!node) return NULL;
    vfs_node_t *vn = kzalloc(sizeof(vfs_node_t));
    if (!vn) return NULL;
    strncpy(vn->name, name, VFS_NAME_MAX - 1);
    vn->type = node->type;
    vn->size = node->size;
    vn->private_data = node;
    return vn;
}

static int tmpfs_vfs_read(vfs_node_t *node, uint64_t offset, void *buf, size_t count) {
    if (!node || !node->private_data) return -EINVAL;
    tmpfs_node_t *tn = node->private_data;
    if (offset >= tn->size) return 0;
    if (offset + count > tn->size) count = tn->size - offset;
    memcpy(buf, tn->data + offset, count);
    return (int)count;
}

static int tmpfs_vfs_write(vfs_node_t *node, uint64_t offset, const void *buf, size_t count) {
    if (!node || !node->private_data) return -EINVAL;
    tmpfs_node_t *tn = node->private_data;
    if (offset + count > tn->capacity) {
        size_t new_cap = tn->capacity ? tn->capacity * 2 : 256;
        while (new_cap < offset + count) new_cap *= 2;
        if (new_cap > TMPFS_MAX_SIZE) return -ENOSPC;
        uint8_t *new_data = krealloc(tn->data, new_cap);
        if (!new_data) return -ENOMEM;
        tn->data = new_data;
        tn->capacity = new_cap;
    }
    memcpy(tn->data + offset, buf, count);
    if (offset + count > tn->size) tn->size = offset + count;
    node->size = tn->size;
    return (int)count;
}

static vfs_ops_t g_tmpfs_ops = {
    .lookup = tmpfs_vfs_lookup,
    .read = tmpfs_vfs_read,
    .write = tmpfs_vfs_write,
    .readdir = NULL,
    .create = NULL,
    .mkdir = NULL,
    .remove = NULL,
};

void tmpfs_init(void) {
    klog_info("TMPFS initialized");
    for (int i = 0; i < TMPFS_MAX_FILES; i++) {
        g_tmpfs_nodes[i].valid = false;
    }
}

bool tmpfs_mount(vfs_ops_t **ops_out) {
    klog_info("TMPFS mounted");
    if (ops_out) *ops_out = &g_tmpfs_ops;
    return true;
}

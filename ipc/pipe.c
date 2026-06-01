/* gOS Kernel — Pipe Implementation
 * Reference: https://wiki.osdev.org/Pipe
 */

#include "ipc/pipe.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

static pipe_t g_pipes[PIPE_MAX_PIPES];

void pipe_init(void) {
    klog_info("Pipe subsystem initialized");
    for (int i = 0; i < PIPE_MAX_PIPES; i++) {
        g_pipes[i].readable = false;
        g_pipes[i].writable = false;
        g_pipes[i].refcount = 0;
    }
}

int pipe_create(pipe_t **read_end, pipe_t **write_end) {
    if (!read_end || !write_end) return -EINVAL;

    int r_idx = -1, w_idx = -1;
    for (int i = 0; i < PIPE_MAX_PIPES; i++) {
        if (g_pipes[i].refcount == 0) {
            if (r_idx < 0) r_idx = i;
            else if (w_idx < 0) w_idx = i;
        }
    }
    if (r_idx < 0 || w_idx < 0) return -ENFILE;

    memset(&g_pipes[r_idx], 0, sizeof(pipe_t));
    memset(&g_pipes[w_idx], 0, sizeof(pipe_t));
    g_pipes[r_idx].readable = true;
    g_pipes[r_idx].refcount = 1;
    g_pipes[w_idx].writable = true;
    g_pipes[w_idx].refcount = 1;

    *read_end = &g_pipes[r_idx];
    *write_end = &g_pipes[w_idx];
    klog_debug("Pipe created: read=%p write=%p", (void *)*read_end, (void *)*write_end);
    return 0;
}

static inline size_t pipe_size(const pipe_t *p) {
    return (p->write_pos >= p->read_pos)
        ? (p->write_pos - p->read_pos)
        : (PIPE_BUF_SIZE - p->read_pos + p->write_pos);
}

bool pipe_empty(const pipe_t *p) {
    return p->read_pos == p->write_pos;
}

bool pipe_full(const pipe_t *p) {
    return ((p->write_pos + 1) % PIPE_BUF_SIZE) == p->read_pos;
}

int pipe_read(pipe_t *p, void *buf, size_t count) {
    if (!p || !buf || !p->readable) return -EINVAL;
    uint8_t *dst = buf;
    size_t read = 0;
    while (read < count && !pipe_empty(p)) {
        dst[read++] = p->buffer[p->read_pos];
        p->read_pos = (p->read_pos + 1) % PIPE_BUF_SIZE;
    }
    return (int)read;
}

int pipe_write(pipe_t *p, const void *buf, size_t count) {
    if (!p || !buf || !p->writable) return -EINVAL;
    const uint8_t *src = buf;
    size_t written = 0;
    while (written < count && !pipe_full(p)) {
        p->buffer[p->write_pos] = src[written++];
        p->write_pos = (p->write_pos + 1) % PIPE_BUF_SIZE;
    }
    return (int)written;
}

void pipe_close(pipe_t *p) {
    if (!p) return;
    p->refcount--;
    if (p->refcount <= 0) {
        p->readable = false;
        p->writable = false;
        p->read_pos = 0;
        p->write_pos = 0;
    }
}

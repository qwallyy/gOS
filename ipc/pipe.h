/* gOS Kernel — Pipes
 * Reference: https://wiki.osdev.org/Pipe
 */

#ifndef GOS_PIPE_H
#define GOS_PIPE_H

#include "gOS/types.h"

#define PIPE_BUF_SIZE       4096
#define PIPE_MAX_PIPES      32

typedef struct pipe {
    uint8_t buffer[PIPE_BUF_SIZE];
    volatile size_t read_pos;
    volatile size_t write_pos;
    bool readable;
    bool writable;
    int refcount;
} pipe_t;

void pipe_init(void);
int  pipe_create(pipe_t **read_end, pipe_t **write_end);
int  pipe_read(pipe_t *p, void *buf, size_t count);
int  pipe_write(pipe_t *p, const void *buf, size_t count);
bool pipe_empty(const pipe_t *p);
bool pipe_full(const pipe_t *p);
void pipe_close(pipe_t *p);

#endif /* GOS_PIPE_H */

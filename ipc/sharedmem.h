/* gOS Kernel — Shared Memory
 * Reference: https://wiki.osdev.org/Shared_Memory
 */

#ifndef GOS_SHAREDMEM_H
#define GOS_SHAREDMEM_H

#include "gOS/types.h"

#define SHM_MAX_REGIONS     16
#define SHM_NAME_MAX        32

typedef struct shm_region {
    char name[SHM_NAME_MAX];
    void *addr;
    size_t size;
    int refcount;
    bool valid;
} shm_region_t;

void shm_init(void);
int  shm_create(const char *name, size_t size, void **out_addr);
int  shm_open(const char *name, void **out_addr);
int  shm_close(const char *name);
void shm_list(void);

#endif /* GOS_SHAREDMEM_H */

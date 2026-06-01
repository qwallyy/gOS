/* gOS Kernel — Initial RAM Disk (Initrd)
 * Reference: https://wiki.osdev.org/Initrd
 */

#ifndef GOS_INITRD_H
#define GOS_INITRD_H

#include "gOS/types.h"

#define INITRD_MAX_FILES    64
#define INITRD_FILENAME_MAX 64

typedef struct initrd_file {
    char name[INITRD_FILENAME_MAX];
    uint32_t offset;
    uint32_t size;
    bool valid;
} initrd_file_t;

typedef struct initrd_header {
    uint32_t magic;
    uint32_t num_files;
} PACKED initrd_header_t;

#define INITRD_MAGIC        0xBADB0021

void initrd_init(uintptr_t start, uintptr_t end);
bool initrd_is_present(void);
const initrd_file_t *initrd_find(const char *name);
size_t initrd_read(const initrd_file_t *file, void *buf, size_t offset, size_t len);
void initrd_list(void);
uintptr_t initrd_get_base(void);
size_t initrd_get_size(void);

#endif /* GOS_INITRD_H */

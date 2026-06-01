/* gOS Kernel — Kernel Module System (stub)
 * Reference: https://wiki.osdev.org/Modules
 */

#ifndef GOS_MODULE_H
#define GOS_MODULE_H

#include "gOS/types.h"

#define MODULE_NAME_MAX     64
#define MODULE_MAX_MODULES  16

typedef struct kernel_module {
    char name[MODULE_NAME_MAX];
    void *base;
    size_t size;
    bool loaded;
    int (*init)(void);
    void (*exit)(void);
} kernel_module_t;

void module_init(void);
int  module_load(const char *name, const void *elf_data, size_t size);
int  module_unload(const char *name);
bool module_is_loaded(const char *name);
void module_list(void);

#endif /* GOS_MODULE_H */

/* gOS Kernel — Kernel Module System Implementation (stub) */

#include "kernel/module.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

static kernel_module_t g_modules[MODULE_MAX_MODULES];

void module_init(void) {
    klog_info("Kernel module system initialized");
    for (int i = 0; i < MODULE_MAX_MODULES; i++) {
        g_modules[i].loaded = false;
    }
}

int module_load(const char *name, const void *elf_data, size_t size) {
    (void)elf_data; (void)size;
    klog_warn("module_load('%s'): not implemented (no ELF relocator yet)", name);
    return -ENOSYS;
}

int module_unload(const char *name) {
    for (int i = 0; i < MODULE_MAX_MODULES; i++) {
        if (g_modules[i].loaded && strcmp(g_modules[i].name, name) == 0) {
            if (g_modules[i].exit) g_modules[i].exit();
            g_modules[i].loaded = false;
            klog_info("Module '%s' unloaded", name);
            return 0;
        }
    }
    return -ENOENT;
}

bool module_is_loaded(const char *name) {
    for (int i = 0; i < MODULE_MAX_MODULES; i++) {
        if (g_modules[i].loaded && strcmp(g_modules[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

void module_list(void) {
    klog_info("Loaded modules:");
    for (int i = 0; i < MODULE_MAX_MODULES; i++) {
        if (g_modules[i].loaded) {
            klog_info("  %s", g_modules[i].name);
        }
    }
}

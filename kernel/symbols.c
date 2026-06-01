/* gOS Kernel — Kernel Symbol Table Implementation (stub)
 * In a full build, this would be populated by the linker script
 * or a post-build symbol extraction step.
 */

#include "kernel/symbols.h"
#include "gOS/klog.h"
#include "lib/string.h"

/* Stub: no real symbols yet */
static kernel_symbol_t g_kernel_symbols[] = {
    { 0xFFFFFFFF80000000ULL, "_start" },
    { 0, NULL }
};

void symbols_init(void) {
    klog_info("Kernel symbol table initialized (stub)");
}

const char *symbols_lookup(uint64_t addr, uint64_t *out_offset) {
    const char *best_name = NULL;
    uint64_t best_addr = 0;

    for (int i = 0; g_kernel_symbols[i].name; i++) {
        if (g_kernel_symbols[i].addr <= addr && g_kernel_symbols[i].addr > best_addr) {
            best_addr = g_kernel_symbols[i].addr;
            best_name = g_kernel_symbols[i].name;
        }
    }

    if (out_offset) {
        *out_offset = addr - best_addr;
    }
    return best_name ? best_name : "<unknown>";
}

uint64_t symbols_resolve(const char *name) {
    for (int i = 0; g_kernel_symbols[i].name; i++) {
        if (strcmp(g_kernel_symbols[i].name, name) == 0) {
            return g_kernel_symbols[i].addr;
        }
    }
    return 0;
}

void symbols_dump(void) {
    klog_info("Kernel symbols:");
    for (int i = 0; g_kernel_symbols[i].name; i++) {
        klog_info("  0x%016llx %s",
                  (unsigned long long)g_kernel_symbols[i].addr,
                  g_kernel_symbols[i].name);
    }
}

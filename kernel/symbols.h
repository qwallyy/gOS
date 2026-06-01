/* gOS Kernel — Kernel Symbol Table (stub)
 * For future ELF symbol resolution in backtraces.
 */

#ifndef GOS_SYMBOLS_H
#define GOS_SYMBOLS_H

#include "gOS/types.h"

typedef struct kernel_symbol {
    uint64_t addr;
    const char *name;
} kernel_symbol_t;

void symbols_init(void);
const char *symbols_lookup(uint64_t addr, uint64_t *out_offset);
uint64_t symbols_resolve(const char *name);
void symbols_dump(void);

#endif /* GOS_SYMBOLS_H */

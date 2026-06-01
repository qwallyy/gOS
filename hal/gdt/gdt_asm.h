/* gOS Kernel — GDT Assembly Helper Declarations */

#ifndef GOS_GDT_ASM_H
#define GOS_GDT_ASM_H

#include "gOS/types.h"

void gdt_load(uint64_t base, uint16_t limit);
void gdt_reload_segments(uint16_t code_sel, uint16_t data_sel);
void gdt_reload_fs_gs(uint16_t fs_sel, uint16_t gs_sel);

#endif /* GOS_GDT_ASM_H */

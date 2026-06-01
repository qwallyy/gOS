/* gOS Kernel — Multiboot2 Parser Declarations */

#ifndef GOS_MULTIBOOT_H
#define GOS_MULTIBOOT_H

#include "gOS/kernel.h"

void parse_multiboot2_info(uintptr_t info_addr);

#endif /* GOS_MULTIBOOT_H */

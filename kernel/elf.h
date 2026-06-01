/* gOS Kernel — ELF64 Loader Definitions
 * Reference: https://wiki.osdev.org/ELF
 *            https://wiki.osdev.org/ELF_Tutorial
 */

#ifndef GOS_ELF_H
#define GOS_ELF_H

#include "gOS/types.h"

#define ELFMAG          "\x7fELF"
#define EI_CLASS        4
#define ELFCLASS64      2
#define EI_DATA         5
#define ELFDATA2LSB     1
#define EV_CURRENT      1
#define ET_EXEC         2
#define ET_DYN          3
#define EM_X86_64       62

#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4

#define PF_X            1
#define PF_W            2
#define PF_R            4

typedef struct elf64_header {
    uint8_t  ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} PACKED elf64_header_t;

typedef struct elf64_program_header {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} PACKED elf64_program_header_t;

typedef struct elf64_section_header {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
} PACKED elf64_section_header_t;

bool elf_is_valid(const void *data);
bool elf_is_64bit(const void *data);
bool elf_is_x86_64(const void *data);
uint64_t elf_get_entry(const void *data);
int elf_load(const void *data, size_t size, uintptr_t *out_entry);

#endif /* GOS_ELF_H */

/* gOS Kernel — ELF64 Loader Implementation
 * Reference: https://wiki.osdev.org/ELF
 */

#include "kernel/elf.h"
#include "mm/vmm/vmm.h"
#include "mm/pmm/pmm.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

bool elf_is_valid(const void *data) {
    if (!data) return false;
    const uint8_t *ident = data;
    return ident[0] == 0x7f && ident[1] == 'E' && ident[2] == 'L' && ident[3] == 'F';
}

bool elf_is_64bit(const void *data) {
    if (!data) return false;
    const uint8_t *ident = data;
    return ident[EI_CLASS] == ELFCLASS64;
}

bool elf_is_x86_64(const void *data) {
    if (!data) return false;
    const elf64_header_t *hdr = data;
    return hdr->machine == EM_X86_64;
}

uint64_t elf_get_entry(const void *data) {
    if (!data) return 0;
    const elf64_header_t *hdr = data;
    return hdr->entry;
}

int elf_load(const void *data, size_t size, uintptr_t *out_entry) {
    if (!data || size < sizeof(elf64_header_t)) return -EINVAL;
    if (!elf_is_valid(data)) return -ENOEXEC;
    if (!elf_is_64bit(data)) return -ENOEXEC;
    if (!elf_is_x86_64(data)) return -ENOEXEC;

    const elf64_header_t *hdr = data;
    klog_info("ELF: loading %u program headers", hdr->phnum);

    for (uint16_t i = 0; i < hdr->phnum; i++) {
        const elf64_program_header_t *ph = (const elf64_program_header_t *)
            ((const uint8_t *)data + hdr->phoff + i * hdr->phentsize);

        if (ph->type != PT_LOAD) continue;

        klog_info("ELF: segment %u: vaddr=0x%llx filesz=%llu memsz=%llu",
                  i, (unsigned long long)ph->vaddr,
                  (unsigned long long)ph->filesz,
                  (unsigned long long)ph->memsz);

        /* Allocate and map pages for this segment */
        size_t pages = (ph->memsz + PAGE_SIZE - 1) / PAGE_SIZE;
        for (size_t p = 0; p < pages; p++) {
            void *phys = pmm_alloc_page();
            if (!phys) return -ENOMEM;
            uint64_t flags = PAGE_PRESENT | PAGE_USER;
            if (ph->flags & PF_W) flags |= PAGE_WRITABLE;
            if (!(ph->flags & PF_X)) flags |= PAGE_NX;
            vmm_map_page(ph->vaddr + p * PAGE_SIZE, (uintptr_t)phys, flags);
        }

        /* Copy file data */
        memcpy((void *)ph->vaddr, (const uint8_t *)data + ph->offset, ph->filesz);
        /* Zero BSS */
        if (ph->memsz > ph->filesz) {
            memset((void *)(ph->vaddr + ph->filesz), 0, ph->memsz - ph->filesz);
        }
    }

    if (out_entry) *out_entry = hdr->entry;
    klog_info("ELF: entry point 0x%llx", (unsigned long long)hdr->entry);
    return 0;
}

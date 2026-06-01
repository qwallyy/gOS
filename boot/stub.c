/*
 * Multiboot1 stub for QEMU direct kernel boot.
 *
 * This stub is loaded by QEMU at 0x90000 (via multiboot1 -kernel).
 * It parses an embedded ELF64 kernel and copies each PT_LOAD segment
 * to its correct physical address, then jumps to the kernel entry point.
 */

#include <stdint.h>

/* Multiboot1 header */
#define MULTIBOOT_MAGIC    0x1BADB002
#define MULTIBOOT_FLAGS    0x00010000  /* flag 16: header_addr and load_addr valid */

typedef struct {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header_t;

/* Place multiboot header at the very beginning */
__attribute__((section(".multiboot1")))
multiboot_header_t mb_header = {
    .magic = MULTIBOOT_MAGIC,
    .flags = MULTIBOOT_FLAGS,
    .checksum = 0xE4422FDEUL,
    .header_addr = 0x100000,
    .load_addr = 0x100000,
    .load_end_addr = 0,  /* 0 = load entire file */
    .bss_end_addr = 0,
    .entry_addr = 0x100020  /* _start_stub entry point */
};

typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

/* The kernel ELF is embedded at a fixed offset from the stub start */
#define KERNEL_OFFSET 0x1000

static inline void halt(void) {
    __asm__ volatile ("cli; hlt");
}

void __attribute__((section(".text.stub"))) _start_stub(void) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)(0x90000 + KERNEL_OFFSET);

    /* Verify ELF64 magic */
    if (ehdr->e_ident[0] != 0x7F ||
        ehdr->e_ident[1] != 'E' ||
        ehdr->e_ident[2] != 'L' ||
        ehdr->e_ident[3] != 'F' ||
        ehdr->e_ident[4] != 2) {  /* ELFCLASS64 */
        halt();
    }

    Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)ehdr + (uint32_t)ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == 1) { /* PT_LOAD */
            uint8_t *src = (uint8_t *)ehdr + (uint32_t)phdr[i].p_offset;
            uint8_t *dst = (uint8_t *)(uint32_t)phdr[i].p_paddr;
            uint32_t filesz = (uint32_t)phdr[i].p_filesz;
            uint32_t memsz  = (uint32_t)phdr[i].p_memsz;

            /* Copy file contents */
            for (uint32_t j = 0; j < filesz; j++) {
                dst[j] = src[j];
            }

            /* Zero the rest */
            for (uint32_t j = filesz; j < memsz; j++) {
                dst[j] = 0;
            }
        }
    }

    /* Jump to kernel entry point */
    void (*entry)(void) = (void (*)(void))(uint32_t)ehdr->e_entry;
    entry();
}

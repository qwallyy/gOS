/* Linker script for multiboot1 stub */
OUTPUT_FORMAT("elf32-i386")
ENTRY(_start_stub)

SECTIONS {
    . = 0x100000;

    /* Multiboot1 header must be in first 8KB */
    .multiboot1 : {
        *(.multiboot1)
    }

    /* Stub code and data */
    .text.stub : {
        *(.text.stub)
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}

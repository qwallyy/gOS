/* gOS Linker Script
 * Reference: https://wiki.osdev.org/Linker_Scripts
 * Sets up the kernel virtual memory layout for x86-64 long mode.
 */

ENTRY(_start)

PHDRS
{
    boot    PT_LOAD FLAGS(5);   /* RX */
    text    PT_LOAD FLAGS(5);   /* RX */
    rodata  PT_LOAD FLAGS(4);   /* R  */
    data    PT_LOAD FLAGS(6);   /* RW */
    note    PT_NOTE;
}

SECTIONS
{
    /* Kernel starts at 1MB physical, mapped to higher half at 0xFFFFFFFF80000000 */
    . = 0x100000;

    /* Multiboot1 header for QEMU -kernel direct boot */
    .multiboot1 ALIGN(4) :
    {
        *(.multiboot1)
    } :boot

    /* Multiboot2 header must be in the first 32KB, aligned to 8 bytes */
    .multiboot2 ALIGN(8) :
    {
        *(.multiboot2)
    } :boot

    /* Boot code runs in low physical memory before paging is enabled */
    .boot :
    {
        *(.boot)
    } :boot

    /* 64-bit boot stubs that must be accessible from 32-bit boot code */
    .text.boot :
    {
        *(.text.boot)
    } :boot

    /* Transition to higher half virtual address */
    . += 0xFFFFFFFF80000000 - 0x100000;

    /* Kernel text/code section */
    .text ALIGN(4K) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
    {
        _text_start = .;
        *(.text)
        *(.text.*)
        . = ALIGN(16);
        _text_end = .;
    _etext = .;
    } :text

    /* Read-only data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
    {
        _rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(16);
        _rodata_end = .;
    } :rodata

    /* Initialized data */
    .data ALIGN(4K) : AT(ADDR(.data) - 0xFFFFFFFF80000000)
    {
        _data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(16);
        _data_end = .;
    _edata = .;
    } :data

    /* Uninitialized data (BSS) */
    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        . = ALIGN(16);
        _bss_end = .;
    _end = .;
    } :data

    /* Kernel end marker */
    _kernel_end = .;

    /* PVH ELF Note for QEMU direct boot */
    .note.Xen :
    {
        *(.note.Xen)
    } :note

    /DISCARD/ :
    {
        *(.comment)
        *(.note)
        *(.note.gnu*)
        *(.note.build-id)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }

}

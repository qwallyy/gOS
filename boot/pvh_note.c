/* PVH ELF Note for QEMU -kernel direct boot */

__asm__(
    ".pushsection .note.Xen, \"a\", @note\n"
    ".align 4\n"
    ".long 4\n"                /* n_namesz */
    ".long 8\n"                /* n_descsz */
    ".long 0x12\n"             /* n_type = XEN_ELFNOTE_PHYS32_ENTRY */
    ".asciz \"Xen\"\n"
    ".align 4\n"
    ".quad 0x101000\n"         /* 64-bit physical entry point */
    ".popsection\n"
);

    .section .note.Xen, "a", @note
    .align 4
    .long 4                 /* n_namesz */
    .long 8                 /* n_descsz */
    .long 0x12              /* n_type = XEN_ELFNOTE_PHYS32_ENTRY */
    .asciz "Xen"
    .align 4
    .quad 0x101000          /* 64-bit physical entry point */

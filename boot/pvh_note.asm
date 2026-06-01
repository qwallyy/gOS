; PVH ELF Note for QEMU -kernel direct boot
; Reference: https://wiki.osdev.org/Xen_PVH
; QEMU looks for a .note.Xen section with type 0x12

section .note.Xen type=note
align 4

    dd 4                    ; n_namesz (including null terminator)
    dd 8                    ; n_descsz (uint64_t entry point)
    dd 0x12                 ; n_type = XEN_ELFNOTE_PHYS32_ENTRY
    db "Xen", 0             ; name
    align 4
    dq 0x101000             ; 64-bit physical entry point (_start)

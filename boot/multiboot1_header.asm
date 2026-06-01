; gOS Multiboot1 Header
; Reference: https://wiki.osdev.org/Multiboot
; This header allows QEMU -kernel to load the kernel directly.

section .multiboot1
align 4

    dd 0x1BADB002           ; Multiboot1 magic
    dd 0x00000000           ; Flags
    dd 0xE4524FFE           ; Checksum: -(magic + flags)

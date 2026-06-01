; gOS Multiboot2 Header
; Reference: https://wiki.osdev.org/Multiboot
; Must be in the first 32KB of the kernel image, 8-byte aligned.

section .multiboot2
align 8

extern _start

; Multiboot2 header magic and structure
; Reference: https://wiki.osdev.org/Multiboot2
; Total size = 0x58 (88 bytes), checksum = 0x17ADAED2
mb2_header:
    dd 0xE85250D6           ; Multiboot2 magic number
    dd 0                    ; Architecture: 0 = i386 (we switch to long mode)
    dd 0x58                 ; Header length
    dd 0x17ADAED2           ; Checksum

    ; Information request tag: ask GRUB for memory map, framebuffer, and ACPI
    align 8
    dw 1                    ; Type: information request
    dw 0                    ; Flags
    dd 28                   ; Size = 8 + 5*4
    dd 4                    ; Basic memory info
    dd 6                    ; Memory map
    dd 8                    ; Framebuffer info
    dd 14                   ; ACPI old RSDP
    dd 15                   ; ACPI new RSDP

    ; Address tag: specify load base (0x100000 = 1MB)
    align 8
    dw 21                   ; Type: image load base physical address
    dw 0                    ; Flags
    dd 12                   ; Size
    dd 0x00100000           ; Load base address

    ; Entry address tag
    align 8
    dw 3                    ; Type: entry address
    dw 0                    ; Flags
    dd 12                   ; Size
    dd _start               ; Entry point

    ; End tag
    align 8
    dw 0                    ; Type: end
    dw 0                    ; Flags
    dd 8                    ; Size

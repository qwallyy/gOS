; gOS Multiboot2 Header
; Reference: https://wiki.osdev.org/Multiboot
; Must be in the first 32KB of the kernel image, 8-byte aligned.

section .multiboot2
align 8

extern _start

; Multiboot2 header magic and structure
; Reference: https://wiki.osdev.org/Multiboot2
; Total size = 0x48 (72 bytes), checksum = 0x17ADAEE2
mb2_header:
    dd 0xE85250D6           ; Multiboot2 magic number
    dd 0                    ; Architecture: 0 = i386 (we switch to long mode)
    dd 0x48                 ; Header length
    dd 0x17ADAEE2           ; Checksum

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

    ; NOTE: there is no valid multiboot2 *header* tag for "load base address".
    ; Type 21 ("image load base physical address") is a *boot information* tag
    ; the bootloader passes TO the kernel — not a header tag. A required header
    ; tag GRUB doesn't recognise makes it abort before launching the kernel
    ; (hence the ISO booting to a blank screen). The ELF program headers already
    ; specify the load addresses, so no address tag is needed here.

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

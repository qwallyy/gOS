; gOS Long Mode Entry — 64-bit Assembly Stubs
; Reference: https://wiki.osdev.org/Setting_Up_Long_Mode
;
; This file handles the transition from compatibility mode to true
; 64-bit long mode, sets up the 64-bit GDT, initializes the kernel
; environment, and calls into C code.

section .text.boot
bits 64

; This file relies on absolute addressing (e.g. `lgdt [gdt64_ptr]`); state it
; explicitly so NASM 3.x doesn't warn that the implicit default is deprecated.
default abs

global long_mode_start
extern kernel_early_init
extern kernel_main

; ---------------------------------------------------------------------------
; long_mode_start — entered from boot.asm via far jump
; ---------------------------------------------------------------------------
long_mode_start:
    ; We are now in 64-bit long mode with paging enabled.
    ; The higher half mapping is active.

    ; Clear segment registers to ensure no stale 32-bit selectors
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up the 64-bit GDT with a flat memory model
    ; Reference: https://wiki.osdev.org/GDT
    lgdt [gdt64_ptr]

    ; Reload CS via far return to ensure the new GDT is active
    push gdt64_kernel_code
    lea rax, [rel .gdt_reload_done]
    push rax
    o64 retf

.gdt_reload_done:
    ; Now using the proper 64-bit kernel code segment

    ; Keep the stack pointer set up by boot.asm (temp_stack_top).
    ; In 64-bit mode RSP is the zero-extension of ESP from boot.asm.

    ; Clear the interrupt flag until the IDT is fully initialized
    ; Reference: https://wiki.osdev.org/IDT
    cli

    ; Save the multiboot2 info physical address on the stack
    ; It was pushed in boot.asm before entering long mode
    pop rdi                 ; RDI = multiboot2 info physical address

    ; Call the early kernel initialization (still in assembly-safe C)
    ; This parses multiboot2, sets up PMM/VMM, and prepares for main()
    movabs rax, kernel_early_init
    call rax

    ; Call the main kernel entry point
    movabs rax, kernel_main
    call rax

    ; kernel_main should never return, but if it does, halt
.halt:
    cli
    hlt
    jmp .halt

; ---------------------------------------------------------------------------
; 64-bit GDT (flat memory model for long mode)
; Reference: https://wiki.osdev.org/GDT
; ---------------------------------------------------------------------------
section .text.boot
align 8

gdt64_flat:
    dq 0                                ; 0x00: Null descriptor

gdt64_kernel_code: equ $ - gdt64_flat
    ; 0x08: 64-bit kernel code segment
    ; Bits: present(47), code/data(44), executable(43), 64-bit(53), ring 0
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)

gdt64_kernel_data: equ $ - gdt64_flat
    ; 0x10: 64-bit kernel data segment
    ; Bits: present(47), code/data(44), writable(41), ring 0
    dq (1 << 41) | (1 << 44) | (1 << 47)

gdt64_user_code32: equ $ - gdt64_flat
    ; 0x18: 32-bit user code (for compatibility mode transitions)
    ; Bits: present(47), code/data(44), executable(43), ring 3(45-46)
    ;       conforming(42), readable(40), granularity(55), 32-bit(54)
    dw 0xFFFF, 0
    db 0
    db 0xFA                 ; Present, ring 3, code, non-conforming, readable
    db 0xCF                 ; 4KB granularity, 32-bit, limit = 0xF
    db 0

gdt64_user_data: equ $ - gdt64_flat
    ; 0x20: 64-bit user data
    ; Bits: present(47), code/data(44), writable(41), ring 3(45-46)
    dq (1 << 41) | (1 << 44) | (1 << 45) | (1 << 46) | (1 << 47)

gdt64_user_code64: equ $ - gdt64_flat
    ; 0x28: 64-bit user code
    ; Bits: present(47), code/data(44), executable(43), 64-bit(53), ring 3
    dq (1 << 43) | (1 << 44) | (1 << 45) | (1 << 46) | (1 << 47) | (1 << 53)

gdt64_tss_low: equ $ - gdt64_flat
    ; 0x30: TSS descriptor low (filled by C code)
    dq 0

gdt64_tss_high: equ $ - gdt64_flat
    ; 0x38: TSS descriptor high (for 64-bit TSS base extension)
    dq 0

gdt64_end:

gdt64_ptr:
    dw gdt64_end - gdt64_flat - 1
    dq gdt64_flat

; ---------------------------------------------------------------------------
; TSS (Task State Segment) — required for Ring 0 -> Ring 3 transitions
; Reference: https://wiki.osdev.org/TSS
; ---------------------------------------------------------------------------
section .bss
alignb 16

global tss64
tss64:
    ; 64-bit TSS structure (104 bytes minimum)
    resw 1                  ; Reserved
    resw 1                  ; RSP0 low (filled by C)
    resd 1                  ; RSP0 high
    resw 1                  ; RSP1 low
    resd 1                  ; RSP1 high
    resw 1                  ; RSP2 low
    resd 1                  ; RSP2 high
    resq 1                  ; Reserved
    resw 1                  ; IST1 low
    resd 1                  ; IST1 high
    resw 1                  ; IST2 low
    resd 1                  ; IST2 high
    resw 1                  ; IST3 low
    resd 1                  ; IST3 high
    resw 1                  ; IST4 low
    resd 1                  ; IST4 high
    resw 1                  ; IST5 low
    resd 1                  ; IST5 high
    resw 1                  ; IST6 low
    resd 1                  ; IST6 high
    resw 1                  ; IST7 low
    resd 1                  ; IST7 high
    resq 1                  ; Reserved
    resw 1                  ; I/O Map Base
    resw 1                  ; Reserved (padding to 104 bytes)
tss64_end:

; Stack for IST1 (used for NMI / double fault handlers)
alignb 4096
ist1_stack:
    resb 8192
ist1_stack_top:

; gOS Kernel — GDT Assembly Helpers
; Reference: https://wiki.osdev.org/GDT

bits 64
section .text

global gdt_load
global gdt_reload_segments
global gdt_reload_fs_gs

gdt_load:
    push rbx
    mov bx, si
    mov rax, rdi
    lgdt [rsp-8]
    pop rbx
    ret

gdt_reload_segments:
    push rsi
    lea rax, [rel .reload_cs]
    push rax
    retfq
.reload_cs:
    mov ax, di
    mov ds, ax
    mov es, ax
    mov ss, ax
    ret

gdt_reload_fs_gs:
    mov ax, di
    mov fs, ax
    mov ax, si
    mov gs, ax
    ret

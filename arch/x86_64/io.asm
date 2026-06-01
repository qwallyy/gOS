; gOS Kernel — I/O Port Operations (Assembly)
; Reference: https://wiki.osdev.org/Inline_Assembly/Examples

bits 64
section .text

global io_inb
global io_inw
global io_inl
global io_outb
global io_outw
global io_outl
global io_wait

io_inb:
    mov dx, di
    in al, dx
    ret

io_inw:
    mov dx, di
    in ax, dx
    ret

io_inl:
    mov dx, di
    in eax, dx
    ret

io_outb:
    mov dx, di
    mov al, sil
    out dx, al
    ret

io_outw:
    mov dx, di
    mov ax, si
    out dx, ax
    ret

io_outl:
    mov dx, di
    mov eax, esi
    out dx, eax
    ret

io_wait:
    jmp .io_wait_delay
.io_wait_delay:
    jmp .io_wait_done
.io_wait_done:
    ret

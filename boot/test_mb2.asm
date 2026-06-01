; NASM places sections in the order they appear when using bin format
; Ensure multiboot1 header is first by putting it in the text section

section .text
    global _start

    ; Multiboot1 header must be within first 8KB
    dd 0x1BADB002
    dd 0x00000000
    dd -(0x1BADB002 + 0x00000000)

_start:
    mov al, 'H'
    mov dx, 0x3F8
    out dx, al
    cli
    hlt
    jmp _start

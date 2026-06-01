section .multiboot1
    dd 0x1BADB002
    dd 0x00000000
    dd -(0x1BADB002 + 0x00000000)

section .text
    global _start
_start:
    cli
    hlt
    jmp _start

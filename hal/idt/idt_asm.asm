; gOS Kernel — IDT Assembly Helpers
; Reference: https://wiki.osdev.org/IDT

bits 64
section .text

global idt_load
global irq_disable
global irq_enable
global irq_halt

idt_load:
    push rbx
    mov bx, si
    mov rax, rdi
    lidt [rsp-8]  ; lidt expects a 10-byte memory operand
    pop rbx
    ret

irq_disable:
    cli
    ret

irq_enable:
    sti
    ret

irq_halt:
    hlt
    ret

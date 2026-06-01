; gOS Kernel — SYSCALL/SYSRET Assembly Setup
; Reference: https://wiki.osdev.org/SYSENTER

bits 64
section .text

global syscall_entry_asm
global syscall_enable
global syscall_disable
extern syscall_dispatch

syscall_entry_asm:
    ; Save user state
    swapgs
    mov gs:[0], rsp         ; Save user stack
    mov rsp, gs:[8]         ; Load kernel stack
    push rax
    push rcx                ; User RIP (saved by SYSCALL)
    push r11                ; User RFLAGS (saved by SYSCALL)
    push rdi
    push rsi
    push rdx
    push rbx
    push rbp
    push r8
    push r9
    push r10
    push r12
    push r13
    push r14
    push r15

    ; Call C handler
    mov rdi, rax            ; Syscall number
    call syscall_dispatch

    ; Restore state
    pop r15
    pop r14
    pop r13
    pop r12
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rbx
    pop rdx
    pop rsi
    pop rdi
    pop r11
    pop rcx
    pop rax
    mov rsp, gs:[0]         ; Restore user stack
    swapgs
    sysret

syscall_enable:
    ret

syscall_disable:
    ret

; gOS ISR Stubs — Assembly Thunks for Interrupt Handling
; Reference: https://wiki.osdev.org/IDT
;            https://wiki.osdev.org/ISR
;
; The IDT requires each entry point to be a valid 64-bit code address.
; For exceptions that push an error code automatically, we provide a
; separate stub. For exceptions without an error code, we push a dummy
; 0 to keep the stack frame uniform. All stubs save the full CPU state
; and call the common C handler.

section .text
bits 64

; External C handler
extern isr_common_handler

; ---------------------------------------------------------------------------
; Macro: ISR stub without error code
; ---------------------------------------------------------------------------
%macro ISR_NOERR 1
    global isr_stub_%1
isr_stub_%1:
    push 0                  ; Dummy error code for uniform stack frame
    push %1                 ; Interrupt vector number
    jmp isr_common_entry
%endmacro

; ---------------------------------------------------------------------------
; Macro: ISR stub with error code
; ---------------------------------------------------------------------------
%macro ISR_ERR 1
    global isr_stub_%1
isr_stub_%1:
    push %1                 ; Interrupt vector number (error code already pushed by CPU)
    jmp isr_common_entry
%endmacro

; ---------------------------------------------------------------------------
; Macro: IRQ stub (remapped PIC IRQs)
; ---------------------------------------------------------------------------
%macro IRQ_STUB 1
    global irq_stub_%1
irq_stub_%1:
    push 0                  ; Dummy error code
    push (%1 + 32)          ; IRQ vector = IRQ number + 32 (after PIC remapping)
    jmp isr_common_entry
%endmacro

; ---------------------------------------------------------------------------
; CPU Exception Stubs (0-31)
; Reference: https://wiki.osdev.org/Exceptions
; ---------------------------------------------------------------------------
ISR_NOERR 0     ; #DE Divide Error
ISR_NOERR 1     ; #DB Debug
ISR_NOERR 2     ; #NMI Non-Maskable Interrupt
ISR_NOERR 3     ; #BP Breakpoint
ISR_NOERR 4     ; #OF Overflow
ISR_NOERR 5     ; #BR Bound Range Exceeded
ISR_NOERR 6     ; #UD Invalid Opcode
ISR_NOERR 7     ; #NM Device Not Available
ISR_ERR   8     ; #DF Double Fault
ISR_NOERR 9     ; Coprocessor Segment Overrun (reserved, no longer used)
ISR_ERR   10    ; #TS Invalid TSS
ISR_ERR   11    ; #NP Segment Not Present
ISR_ERR   12    ; #SS Stack-Segment Fault
ISR_ERR   13    ; #GP General Protection Fault
ISR_ERR   14    ; #PF Page Fault
ISR_NOERR 15    ; Reserved
ISR_NOERR 16    ; #MF x87 FPU Floating-Point Error
ISR_ERR   17    ; #AC Alignment Check
ISR_NOERR 18    ; #MC Machine Check
ISR_NOERR 19    ; #XF SIMD Floating-Point Exception
ISR_NOERR 20    ; #VE Virtualization Exception
ISR_NOERR 21    ; #CP Control Protection Exception
ISR_NOERR 22    ; Reserved
ISR_NOERR 23    ; Reserved
ISR_NOERR 24    ; Reserved
ISR_NOERR 25    ; Reserved
ISR_NOERR 26    ; Reserved
ISR_NOERR 27    ; Reserved
ISR_NOERR 28    ; Reserved
ISR_NOERR 29    ; #HV Hypervisor Injection Exception
ISR_ERR   30    ; #VC VMM Communication Exception
ISR_NOERR 31    ; Reserved

; ---------------------------------------------------------------------------
; IRQ Stubs (0-15, remapped to vectors 32-47)
; Reference: https://wiki.osdev.org/8259_PIC
; ---------------------------------------------------------------------------
IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

; ---------------------------------------------------------------------------
; Software interrupt stubs (for syscalls or drivers)
; ---------------------------------------------------------------------------
ISR_NOERR 80    ; INT 0x50 — System call entry (legacy)
ISR_NOERR 128   ; INT 0x80 — Alternate syscall entry

; ---------------------------------------------------------------------------
; Common entry point for all ISRs
; ---------------------------------------------------------------------------
isr_common_entry:
    ; Save all general-purpose registers in a predictable order
    ; This creates an `registers_t` structure on the stack.
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Save segment registers (they are 16-bit, but push as 64-bit for alignment)
    mov ax, ds
    push rax
    mov ax, es
    push rax
    mov ax, fs
    push rax
    mov ax, gs
    push rax

    ; Load kernel data segment into all segment registers
    ; This is safe because we are in Ring 0 after any interrupt
    mov ax, 0x10            ; Kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; RDI = pointer to saved register structure (top of stack after pushes)
    mov rdi, rsp

    ; Call the C common handler
    call isr_common_handler

    ; Restore segment registers
    pop rax
    mov gs, ax
    pop rax
    mov fs, ax
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    ; Restore all general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Remove vector number and error code from stack
    add rsp, 16

    ; Return from interrupt — restores RIP, CS, RFLAGS, RSP, SS
    iretq

; ---------------------------------------------------------------------------
; Global stub table for C code to reference when building the IDT
; ---------------------------------------------------------------------------
section .rodata
global isr_stub_table
global irq_stub_table
align 8

isr_stub_table:
%assign i 0
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep

irq_stub_table:
%assign i 0
%rep 16
    dq irq_stub_%+i
%assign i i+1
%endrep

; Additional software interrupt stubs
    dq isr_stub_80
    dq isr_stub_128

; Fill remaining entries up to 256 for full IDT coverage
%assign i 129
%rep 127
    dq 0
%assign i i+1
%endrep

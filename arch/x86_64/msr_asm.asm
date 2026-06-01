; gOS Kernel — MSR Read/Write Assembly
; Reference: https://wiki.osdev.org/Model_Specific_Registers

bits 64
section .text

global read_msr
global write_msr
global read_tsc
global read_rdtscp

read_msr:
    mov ecx, edi
    rdmsr
    shl rdx, 32
    or rax, rdx
    ret

write_msr:
    mov ecx, edi
    mov eax, esi
    shr rsi, 32
    mov edx, esi
    wrmsr
    ret

read_tsc:
    rdtsc
    shl rdx, 32
    or rax, rdx
    ret

read_rdtscp:
    rdtscp
    shl rdx, 32
    or rax, rdx
    ret

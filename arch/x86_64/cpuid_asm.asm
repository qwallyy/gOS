; gOS Kernel — CPUID Assembly Wrapper
; Reference: https://wiki.osdev.org/CPUID

bits 64
section .text

global cpuid_get_vendor
global cpuid_get_brand
global cpuid_get_features

cpuid_get_vendor:
    push rbx
    mov eax, 0
    cpuid
    mov [rdi], ebx
    mov [rdi+4], edx
    mov [rdi+8], ecx
    mov byte [rdi+12], 0
    pop rbx
    ret

cpuid_get_brand:
    push rbx
    mov r8, rdi
    mov ecx, 3
.brand_loop:
    mov eax, 0x80000002
    add eax, 3
    sub eax, ecx
    cpuid
    mov [r8], eax
    mov [r8+4], ebx
    mov [r8+8], edx
    mov [r8+12], ecx
    add r8, 16
    loop .brand_loop
    mov byte [r8], 0
    pop rbx
    ret

cpuid_get_features:
    push rbx
    mov eax, 1
    cpuid
    mov [rdi], edx
    mov [rdi+4], ecx
    pop rbx
    ret

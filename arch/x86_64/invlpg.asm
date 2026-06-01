; gOS Kernel — INVLPG / Cache Operations
; Reference: https://wiki.osdev.org/TLB

bits 64
section .text

global invlpg_page
global flush_tlb
global flush_tlb_entry
global wbinvd_cache

invlpg_page:
    invlpg [rdi]
    ret

flush_tlb:
    mov rax, cr3
    mov cr3, rax
    ret

flush_tlb_entry:
    invlpg [rdi]
    ret

wbinvd_cache:
    wbinvd
    ret

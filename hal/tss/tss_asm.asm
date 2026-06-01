; gOS Kernel — TSS Assembly Helpers
; Reference: https://wiki.osdev.org/Task_State_Segment

bits 64
section .text

global tss_flush

tss_flush:
    ltr di
    ret

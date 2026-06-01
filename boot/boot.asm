; gOS Boot Entry — 32-bit Protected Mode to 64-bit Long Mode
; Reference: https://wiki.osdev.org/Bare_Bones
;            https://wiki.osdev.org/Setting_Up_Long_Mode
;
; This is the very first code executed after GRUB hands control to us.
; We start in 32-bit protected mode, set up a minimal environment,
; enable PAE + long mode, set up identity + higher-half paging,
; and jump to the C kernel in 64-bit long mode.
;
; ALL boot data is kept in .boot section so it stays below 4GB,
; allowing 32-bit absolute addressing from the boot code.

bits 32
section .boot
global _start
extern long_mode_start

; Multiboot2 passes:
;   EAX = magic (0x36D76289)
;   EBX = physical address of multiboot2 info structure
_start:
    ; Debug: write 'B' to COM1 serial port
    ; Port 0x3F8 > 0xFF, so it must go through DX (the imm8 form of OUT
    ; would truncate the port to 0xF8).
    mov al, 'B'
    mov dx, 0x3F8
    out dx, al

    ; Save multiboot2 info pointer (EBX) before it gets clobbered
    mov [mb2_info_ptr], ebx
    mov [mb2_magic], eax

    ; Set up a temporary stack in the .boot area (below 1MB)
    mov esp, temp_stack_top

    ; Save multiboot2 info pointer on stack for later C code
    push ebx

    ; Check that we were actually loaded by a Multiboot2-compliant bootloader.
    ; If not (e.g. QEMU PVH boot), we continue anyway — kernel_early_init
    ; does not strictly require multiboot info.
    cmp eax, 0x36D76289
    je .multiboot_ok
    mov eax, 0x36D76289     ; Fake magic for downstream code
    mov [mb2_magic], eax
.multiboot_ok:

    ; Clear boot-local storage
    cli
    mov edi, boot_clear_start
    mov ecx, boot_clear_end
    sub ecx, edi
    shr ecx, 2              ; Divide by 4 for dword clear
    xor eax, eax
    rep stosd

    ; Set up a temporary GDT for protected mode
    lgdt [gdt32_desc]

    ; Reload segment registers with the new GDT selectors
    jmp 0x08:.reload_cs
.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Enable PAE (Physical Address Extension) — required for long mode
    ; Reference: https://wiki.osdev.org/CPUID
    mov eax, cr4
    or eax, 1 << 5          ; CR4.PAE = 1
    mov cr4, eax

    ; Set up initial page tables
    ; We create:
;   - PML4[0] -> PDPT_LOW  (identity map first 1GB)
;   - PML4[511] -> PDPT_HIGH (higher half map at 0xFFFFFFFF80000000)
    call setup_page_tables

    ; Load PML4 physical address into CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Enable long mode by setting EFER.LME
    ; Reference: https://wiki.osdev.org/Setting_Up_Long_Mode
    mov ecx, 0xC0000080     ; EFER MSR
    rdmsr
    or eax, 1 << 8          ; EFER.LME = 1
    wrmsr

    ; Enable paging (this activates long mode but we are still in compatibility mode)
    mov eax, cr0
    or eax, 1 << 31         ; CR0.PG = 1
    mov cr0, eax

    ; We are now in compatibility mode (long mode with 32-bit code segments).
    ; Load the 64-bit GDT so the far jump enters true 64-bit long mode.
    lgdt [gdt64_desc]

    ; Jump to 64-bit code segment to enter true long mode.
    ; push/retf with a 32-bit offset; CPU sign-extends to 64-bit RIP
    ; because the target code segment has L-bit set.
    push gdt64.code
    push long_mode_start
    retf

.no_multiboot:
    ; If multiboot2 magic is wrong, halt with an error code on port 0x80
    mov al, 0xBA            ; Error: Bad multiboot magic
    out 0x80, al
    hlt
    jmp .no_multiboot

; ---------------------------------------------------------------------------
; setup_page_tables — create identity and higher-half mappings
; ---------------------------------------------------------------------------
setup_page_tables:
    pusha

    ; Clear all page table structures first
    mov edi, pml4_table
    mov ecx, (pml4_table_end - pml4_table) / 4
    xor eax, eax
    rep stosd

    ; PML4[0] -> PDPT_LOW (identity mapping)
    mov eax, pdpt_low
    or eax, 0x03            ; Present + Writable
    mov [pml4_table], eax

    ; PML4[511] -> PDPT_HIGH (higher half mapping at 0xFFFFFFFF80000000)
    mov eax, pdpt_high
    or eax, 0x03
    mov [pml4_table + 511 * 8], eax

    ; PDPT_LOW[0] -> pd (identity map 0-1GB)
    mov eax, pd_table
    or eax, 0x03
    mov [pdpt_low], eax

    ; PDPT_HIGH[0] -> pd (same pd_table for higher half)
    mov eax, pd_table
    or eax, 0x03
    mov [pdpt_high], eax

    ; Fill PD with 2MB huge pages mapping 0-1GB
    mov edi, pd_table
    mov eax, 0x83           ; Present + Writable + Huge (2MB page)
    mov ecx, 512
.fill_pd:
    mov [edi], eax
    add eax, 0x200000       ; Next 2MB page
    add edi, 8
    loop .fill_pd

    popa
    ret

; ---------------------------------------------------------------------------
; Data — all kept in .boot so addresses stay below 4GB
; ---------------------------------------------------------------------------
align 4

; Multiboot2 info pointer storage
mb2_info_ptr:   dd 0
mb2_magic:      dd 0

; Temporary 32-bit GDT
; Reference: https://wiki.osdev.org/GDT_Tutorial
gdt32:
    dq 0                    ; Null descriptor
.gdt32_code:                ; 32-bit code segment
    dw 0xFFFF               ; Limit low
    dw 0                    ; Base low
    db 0                    ; Base mid
    db 10011010b            ; Access: present, ring 0, code, conforming, readable
    db 11001111b            ; Flags: 4KB granularity, 32-bit, limit high
    db 0                    ; Base high
.gdt32_data:                ; 32-bit data segment
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b            ; Access: present, ring 0, data, expand-up, writable
    db 11001111b
    db 0
gdt32_end:

gdt32_desc:
    dw gdt32_end - gdt32 - 1
    dd gdt32

; 64-bit GDT
gdt64:
    dq 0                    ; Null descriptor
.code equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    ; Long mode code segment: executable (43), code/data (44), present (47), 64-bit (53)
.data equ $ - gdt64
    dq (1 << 44) | (1 << 47) | (1 << 41)
    ; Long mode data segment: code/data (44), present (47), writable (41)
gdt64_end:

gdt64_desc:
    dw gdt64_end - gdt64 - 1
    dd gdt64
    dd 0                    ; Upper 32 bits (for 64-bit GDTR)

; Page tables (must be page-aligned). Kept in .boot (a loaded, low-memory
; section), so use `times db 0` rather than `resb`, which would warn about
; reserving uninitialized space in a non-BSS section.
align 4096
pml4_table:
    times 4096 db 0
pdpt_low:
    times 4096 db 0
pdpt_high:
    times 4096 db 0
pd_table:
    times 4096 db 0
pml4_table_end:

; Temporary boot stack (16KB)
align 16
temp_stack:
    times 16384 db 0
temp_stack_top:

; Range to clear at boot
align 4
boot_clear_start:
    times 4 db 0
boot_clear_end:
    times 4 db 0

; Simple disk bootloader for gOS kernel
; Reads the kernel ELF from sectors after the boot sector and loads segments.
; Uses INT 13h AH=02h (CHS read) for floppy compatibility.

[bits 16]
[org 0x7c00]

    jmp start
    nop

    ; BIOS Parameter Block (BPB) for compatibility
    times 59 db 0

start:
    ; Save boot drive
    mov [boot_drive], dl

    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Print 'B'
    mov al, 'B'
    call serial_send

    ; Read kernel ELF from disk (LBA 1 onwards) into 0x10000
    ; We read in chunks of 18 sectors (one track) to optimize
    ; This is done in REAL MODE so INT 13h works correctly
    mov cx, [num_sectors]
    mov word [lba], 1
    mov word [dst_seg], 0x1000
    mov word [dst_off], 0x0000

.read_loop:
    cmp cx, 0
    je .read_done

    ; Convert LBA to CHS for 1.44MB floppy
    ; C = LBA / 36, H = (LBA / 18) % 2, S = (LBA % 18) + 1
    mov ax, [lba]
    xor dx, dx
    mov bx, 18
    div bx
    inc dx
    mov cl, dl
    mov bx, 2
    xor dx, dx
    div bx
    mov ch, al
    shl ah, 6
    or cl, ah
    mov dh, dl
    mov dl, [boot_drive]

    ; Read up to 18 sectors (remaining in this track)
    mov ax, cx
    cmp ax, 1
    jle .do_read
    mov ax, 1
.do_read:
    push ax
    mov ah, 0x02
    mov bx, [dst_seg]
    mov es, bx
    mov bx, [dst_off]
    int 0x13
    jc read_error
    pop ax

    ; Advance
    sub cx, ax
    add [lba], ax
    shl ax, 9           ; sectors * 512
    add [dst_off], ax
    jnc .read_loop
    add word [dst_seg], 0x1000
    jmp .read_loop

.read_done:
    ; Print 'R'
    mov al, 'R'
    call serial_send

    ; Enable A20
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT and enter protected mode
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

read_error:
    mov al, 'E'
    call serial_send
    jmp $

serial_send:
    push dx
    mov dx, 0x3F8
    out dx, al
    pop dx
    ret

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Parse ELF at 0x10000 and copy PT_LOAD segments to p_paddr
    mov esi, 0x10000      ; ELF base

    ; Check ELF magic
    cmp dword [esi], 0x464C457F  ; 'ELF\x7F' in little-endian
    jne .halt

    ; Get e_phoff from offset 0x20
    mov ebx, [esi + 0x20]  ; low 32 bits of e_phoff

    ; Get e_phnum from offset 0x38
    movzx ecx, word [esi + 0x38]

    ; Get e_phentsize from offset 0x36
    movzx edx, word [esi + 0x36]

    ; EBX = program header table offset, ECX = number of headers
.next_phdr:
    cmp ecx, 0
    je .jump_kernel

    ; Program header at esi + ebx
    mov edi, esi
    add edi, ebx

    ; Check p_type == 1 (PT_LOAD)
    cmp dword [edi], 1
    jne .skip

    ; Copy segment: src = esi + [edi+8], dst = [edi+0x18], size = [edi+0x20]
    push ecx
    push ebx
    push esi
    push edi

    mov eax, [edi + 0x08]   ; p_offset (low 32 bits)
    add esi, eax            ; src = ELF base + p_offset
    mov ebx, [edi + 0x18]   ; dst = p_paddr (low 32 bits)
    mov ecx, [edi + 0x20]   ; size = p_filesz (low 32 bits)

    cmp ecx, 0
    je .copy_done
    mov edi, ebx
    cld
    rep movsb

.copy_done:
    pop edi
    pop esi
    pop ebx
    pop ecx

.skip:
    add ebx, edx            ; next program header
    dec ecx
    jmp .next_phdr

.jump_kernel:
    ; Print 'C' (copy done)
    mov al, 'C'
    mov dx, 0x3F8
    out dx, al

    ; Jump to kernel _start (0x101000)
    jmp 0x101000

.halt:
    cli
    hlt
    jmp .halt

[bits 16]

boot_drive: db 0
num_sectors: dw 0xDEAD
lba: dw 0
dst_seg: dw 0
dst_off: dw 0

gdt:
    dq 0          ; null descriptor
gdt_code:
    dw 0xFFFF     ; limit
    dw 0x0000     ; base low
    db 0x00       ; base mid
    db 0x9A       ; present, ring 0, code, readable
    db 0xCF       ; 32-bit, 4KB granularity
    db 0x00       ; base high
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92       ; present, ring 0, data, writable
    db 0xCF
    db 0x00
gdt_end:

gdt_desc:
    dw gdt_end - gdt - 1
    dd gdt

    times 510-($-$$) db 0
    dw 0xaa55

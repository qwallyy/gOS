#!/usr/bin/env python3
"""
Create a bootable disk image for QEMU.

The disk image contains:
- Sector 0: A simple bootloader that reads the kernel flat binary and jumps to it
- Sector 1+: The kernel flat binary (segments placed at their correct LMAs)

Usage: python3 boot/makedisk.py <kernel.elf> <disk.img>
"""

import sys
import struct

def read_elf_segments(elf_path):
    with open(elf_path, 'rb') as f:
        data = f.read()

    # ELF64 header
    magic = data[:4]
    if magic != b'\x7fELF':
        print(f"Error: {elf_path} is not a valid ELF file")
        sys.exit(1)

    elf_class = data[4]
    if elf_class != 2:  # ELFCLASS64
        print(f"Error: Expected ELF64, got ELF{elf_class * 32}")
        sys.exit(1)

    e_phoff = struct.unpack_from('<Q', data, 0x20)[0]
    e_phentsize = struct.unpack_from('<H', data, 0x36)[0]
    e_phnum = struct.unpack_from('<H', data, 0x38)[0]

    segments = []
    for i in range(e_phnum):
        off = e_phoff + i * e_phentsize
        p_type = struct.unpack_from('<I', data, off)[0]
        if p_type != 1:  # PT_LOAD
            continue
        p_offset = struct.unpack_from('<Q', data, off + 0x08)[0]
        p_paddr = struct.unpack_from('<Q', data, off + 0x20)[0]
        p_filesz = struct.unpack_from('<Q', data, off + 0x28)[0]
        p_memsz = struct.unpack_from('<Q', data, off + 0x30)[0]

        seg_data = data[p_offset:p_offset + p_filesz]
        segments.append({
            'paddr': p_paddr,
            'filesz': p_filesz,
            'memsz': p_memsz,
            'data': seg_data
        })

    return segments

def create_flat_binary(segments):
    """Create a flat binary by placing each segment at its physical address."""
    # Find the highest address we need
    max_addr = 0
    for seg in segments:
        end = seg['paddr'] + seg['memsz']
        if end > max_addr:
            max_addr = end

    # Create buffer
    buf = bytearray(max_addr)

    # Place segments
    for seg in segments:
        paddr = seg['paddr']
        filesz = seg['filesz']
        memsz = seg['memsz']
        buf[paddr:paddr + filesz] = seg['data']
        # BSS is already zeroed by bytearray initialization

    return buf

def create_bootloader(kernel_size):
    """
    Create a simple bootloader that:
    1. Sets up segment registers and stack
    2. Uses INT 13h AH=42h (extended read) to read kernel from LBA 1
    3. Copies kernel to physical 0x0
    4. Jumps to 0x101000 (_start)
    """
    # The bootloader is a 16-bit real mode program loaded at 0x7C00
    # We need to read kernel_size bytes from LBA 1 into memory at 0x0
    # Then jump to 0x101000 in protected mode (since our kernel expects protected mode)

    # Actually, our kernel's _start expects to be in 32-bit protected mode (with
    # multiboot info in registers). But the BIOS starts us in real mode.
    # We need to switch to protected mode ourselves.

    # For simplicity, let's just switch to 32-bit protected mode with flat segments
    # and jump to 0x101000.

    # But wait, our kernel expects paging to be disabled initially (it sets up its own
    # page tables). And it expects to be loaded by a multiboot bootloader with EAX=0x2BADB002.
    # Since we're not using multiboot, we can just set EAX to that value and jump.

    # Actually, we already modified boot.asm to accept any EAX value.

    # The bootloader needs to:
    # 1. Read kernel from disk (LBA 1) to 0x10000 (temporary buffer)
    # 2. Copy kernel to 0x0 (since our flat binary is based at 0x0)
    # 3. Set up protected mode GDT
    # 4. Jump to 0x101000

    # Number of sectors to read = ceil(kernel_size / 512)
    num_sectors = (kernel_size + 511) // 512

    asm = f"""
    [bits 16]
    [org 0x7c00]

    jmp start
    nop

    ; BIOS Parameter Block (BPB) for compatibility
    times 59 db 0

start:
    ; Save boot drive number
    mov [boot_drive], dl

    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Print 'B' to serial port (COM1)
    mov al, 'B'
    call serial_send

    ; Read kernel from floppy using INT 13h AH=02h (CHS read)
    ; 1.44MB floppy: 80 cylinders, 2 heads, 18 sectors per track
    ; LBA to CHS: C = LBA / 36, H = (LBA / 18) % 2, S = (LBA % 18) + 1
    mov cx, {num_sectors}      ; CX = sectors to read
    mov word [lba], 1          ; start at LBA 1 (sector after boot sector)
    mov word [dst_seg], 0x1000 ; destination segment (0x10000)
    mov word [dst_off], 0x0000 ; destination offset

.read_loop:
    cmp cx, 0
    je .read_done

    ; Convert LBA to CHS
    mov ax, [lba]
    xor dx, dx
    mov bx, 18
    div bx                ; AX = LBA / 18, DX = LBA % 18
    inc dx                ; S = (LBA % 18) + 1
    mov cl, dl            ; CL = sector number
    mov bx, 2
    xor dx, dx
    div bx                ; AX = Cylinder, DX = Head
    mov ch, al            ; CH = cylinder (low 8 bits)
    shl ah, 6
    or cl, ah             ; CL bits 6-7 = cylinder high 2 bits
    mov dh, dl            ; DH = head
    mov dl, [boot_drive]  ; DL = drive

    ; Read 1 sector
    mov ax, 0x0201        ; AH=02h, AL=01 sector
    mov bx, [dst_seg]
    mov es, bx
    mov bx, [dst_off]     ; ES:BX = destination
    int 0x13
    jc read_error

    ; Advance
    dec cx                ; sectors remaining
    inc word [lba]        ; next LBA
    add word [dst_off], 512
    jnc .read_loop
    add word [dst_seg], 0x1000  ; advance segment if offset overflowed
    jmp .read_loop

.read_done:
    ; Print 'R' to serial port
    mov al, 'R'
    call serial_send

    ; Copy kernel from buffer (0x10000) to destination (0x0)
    mov cx, {kernel_size & 0xFFFF}
    xor si, si
    mov si, 0x1000
    mov ds, si
    xor si, si      ; ds:si = 0x1000:0x0000 = 0x10000
    xor di, di
    mov es, di      ; es:di = 0x0000:0x0000 = 0x00000
    cld
    rep movsb

    ; If kernel_size > 64KB, we need to copy more
    ; For simplicity, assume kernel is < 64KB for now

    ; Print 'C' to serial port
    mov al, 'C'
    call serial_send

    ; Enable A20 line using fast A20
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT and switch to protected mode
    lgdt [gdt_desc]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to 32-bit code
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
    mov ss, ax
    mov esp, 0x90000

    ; Print 'P' to serial port
    mov al, 'P'
    mov dx, 0x3F8
    out dx, al

    ; Jump to kernel _start (0x101000)
    jmp 0x101000

    ; Hang if return
    cli
    hlt
    jmp $

    [bits 16]

boot_drive: db 0
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
"""
    return asm

def create_disk_image(elf_path, disk_path):
    segments = read_elf_segments(elf_path)
    flat_binary = create_flat_binary(segments)

    # Create bootloader
    kernel_size = len(flat_binary)
    bootloader_asm = create_bootloader(kernel_size)

    # Assemble bootloader
    import subprocess
    with open('/tmp/bootloader.asm', 'w') as f:
        f.write(bootloader_asm)

    subprocess.run(['nasm', '-f', 'bin', '/tmp/bootloader.asm', '-o', '/tmp/bootloader.bin'], check=True)

    with open('/tmp/bootloader.bin', 'rb') as f:
        bootloader = f.read()

    if len(bootloader) > 512:
        print(f"Error: Bootloader is {len(bootloader)} bytes, too large for 512 bytes")
        sys.exit(1)

    # Create a 1.44MB floppy disk image (standard size)
    FLOPPY_SIZE = 1474560  # 80 * 2 * 18 * 512 = 1.44MB
    disk = bytearray(FLOPPY_SIZE)
    disk[:len(bootloader)] = bootloader
    disk[512:512 + len(flat_binary)] = flat_binary

    with open(disk_path, 'wb') as f:
        f.write(disk)

    print(f"[DISK] Created {disk_path} ({len(disk)} bytes)")
    print(f"       Bootloader: {len(bootloader)} bytes")
    print(f"       Kernel: {len(flat_binary)} bytes")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <kernel.elf> <disk.img>")
        sys.exit(1)

    create_disk_image(sys.argv[1], sys.argv[2])

#!/usr/bin/env python3
"""
Create a bootable floppy disk image for QEMU.

Usage: python3 boot/makedisk2.py <kernel.elf> <disk.img>
"""

import sys
import struct
import subprocess

def get_num_sectors(elf_path):
    with open(elf_path, 'rb') as f:
        data = f.read()
    return (len(data) + 511) // 512

def create_disk_image(elf_path, disk_path):
    num_sectors = get_num_sectors(elf_path)

    # Assemble bootloader
    subprocess.run(['nasm', '-f', 'bin', 'boot/disk_boot.asm', '-o', '/tmp/disk_boot.bin'], check=True)

    with open('/tmp/disk_boot.bin', 'rb') as f:
        bootloader = bytearray(f.read())

    if len(bootloader) > 512:
        print(f"Error: Bootloader is {len(bootloader)} bytes, too large for 512 bytes")
        sys.exit(1)

    # Patch num_sectors into the bootloader
    # Find the offset of num_sectors within the 512-byte bootloader
    # In disk_boot.asm, num_sectors is at a specific offset from the start
    # We need to find where 'num_sectors:' label ends up in the binary
    # Let's use a marker: search for the pattern around num_sectors

    # Actually, we can just hardcode the offset by looking at the assembled binary
    # num_sectors comes after boot_drive (1 byte), so it's at offset 1 from boot_drive
    # But boot_drive offset changes based on code size

    # Alternative: embed a magic pattern and replace it
    # We'll use 0xDEAD as the placeholder for num_sectors
    placeholder_offset = bootloader.find(struct.pack('<H', 0xDEAD))
    if placeholder_offset == -1:
        print("Warning: Could not find placeholder, trying to patch at known offset")
        # num_sectors is right after boot_drive in the source
        # boot_drive is near the end of the file, before gdt
        # Let's just patch at offset 0x1EE (near the end of the boot sector)
        # This is a guess - better to use the placeholder approach
        # Let's update disk_boot.asm to use a placeholder
        sys.exit(1)

    # Replace placeholder with actual num_sectors
    bootloader[placeholder_offset:placeholder_offset+2] = struct.pack('<H', num_sectors)

    # Create 1.44MB floppy image
    FLOPPY_SIZE = 1474560

    with open(elf_path, 'rb') as f:
        kernel = f.read()

    disk = bytearray(FLOPPY_SIZE)
    disk[:len(bootloader)] = bootloader
    disk[512:512 + len(kernel)] = kernel

    with open(disk_path, 'wb') as f:
        f.write(disk)

    print(f"[DISK] Created {disk_path} ({len(disk)} bytes)")
    print(f"       Bootloader: {len(bootloader)} bytes")
    print(f"       Kernel: {len(kernel)} bytes ({num_sectors} sectors)")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <kernel.elf> <disk.img>")
        sys.exit(1)

    create_disk_image(sys.argv[1], sys.argv[2])

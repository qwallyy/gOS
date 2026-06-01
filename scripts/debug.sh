#!/bin/bash
# gOS QEMU GDB Debug Script
# Reference: https://wiki.osdev.org/QEMU#GDB

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

if [ ! -f "gOS.iso" ]; then
    echo "ERROR: gOS.iso not found. Run 'make iso' first."
    exit 1
fi

if [ ! -f "build/kernel.elf" ]; then
    echo "ERROR: build/kernel.elf not found. Run 'make' first."
    exit 1
fi

echo "Starting QEMU with GDB stub on :1234..."
echo "Run in another terminal:"
echo "  gdb -ex 'target remote :1234' -ex 'set arch i386:x86-64' build/kernel.elf"
echo ""

qemu-system-x86_64 \
    -cdrom gOS.iso \
    -m 256M \
    -serial stdio \
    -s -S \
    -no-reboot \
    -display sdl

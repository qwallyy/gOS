#!/bin/bash
# gOS QEMU Run Script
# Reference: https://wiki.osdev.org/QEMU

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

cd "$PROJECT_ROOT"

if [ ! -f "gOS.iso" ]; then
    echo "ERROR: gOS.iso not found. Run 'make iso' first."
    exit 1
fi

echo "Starting gOS in QEMU..."

qemu-system-x86_64 \
    -cdrom gOS.iso \
    -m 256M \
    -serial stdio \
    -d int,cpu_reset \
    -no-reboot \
    -no-shutdown \
    -display sdl \
    "$@"

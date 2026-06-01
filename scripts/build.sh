#!/bin/bash
# gOS Build Script
# Reference: https://wiki.osdev.org/GCC_Cross-Compiler

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "================================"
echo "  gOS Build Script"
echo "================================"

cd "$PROJECT_ROOT"

# Verify cross-compiler exists
if ! command -v x86_64-elf-gcc &> /dev/null; then
    echo "ERROR: x86_64-elf-gcc not found in PATH"
    echo "Please install a cross-compiler:"
    echo "  https://wiki.osdev.org/GCC_Cross-Compiler"
    exit 1
fi

if ! command -v nasm &> /dev/null; then
    echo "ERROR: nasm not found in PATH"
    exit 1
fi

echo "Toolchain:"
echo "  CC = $(x86_64-elf-gcc --version | head -n1)"
echo "  LD = $(x86_64-elf-ld --version | head -n1)"
echo "  NASM = $(nasm --version | head -n1)"
echo ""

# Clean previous build
make clean

# Build kernel
echo "Building kernel..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Build complete: build/kernel.elf"

# Build ISO if grub-mkrescue is available
if command -v grub-mkrescue &> /dev/null || command -v grub2-mkrescue &> /dev/null || command -v xorriso &> /dev/null; then
    echo "Building ISO..."
    make iso
    echo "ISO complete: gOS.iso"
else
    echo "WARNING: grub-mkrescue / xorriso not found, skipping ISO build"
fi

echo ""
echo "Done."

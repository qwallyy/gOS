#!/usr/bin/env python3
"""Pad stub binary to exactly 4096 bytes, then append kernel ELF."""
import sys

if len(sys.argv) != 3:
    print(f"Usage: {sys.argv[0]} <stub.bin> <kernel.elf>")
    sys.exit(1)

stub_path = sys.argv[1]
kernel_path = sys.argv[2]

with open(stub_path, 'rb') as f:
    stub = f.read()

# Pad to exactly 4096 bytes
if len(stub) < 4096:
    stub += b'\x00' * (4096 - len(stub))
else:
    print(f"Warning: stub is {len(stub)} bytes, larger than 4096")

with open(kernel_path, 'rb') as f:
    kernel = f.read()

with open(stub_path, 'wb') as f:
    f.write(stub)
    f.write(kernel)

print(f"[BOOT] Created {stub_path} ({len(stub) + len(kernel)} bytes)")

# gOS Porting Guide

## Architecture Support

Currently only x86-64 (AMD64) is supported. To port to a new architecture:

### 1. Boot Code

Replace `boot/boot.asm` and `arch/x86_64/long_mode.asm` with architecture-
specific boot sequence. Requirements:
- Set up initial stack
- Transition to target architecture's privileged mode
- Clear BSS
- Jump to `kernel_early_init()`

### 2. HAL Layer

Implement architecture-specific versions of:
- `hal/gdt/` → Architecture descriptor tables
- `hal/idt/` → Interrupt/exception handling
- `hal/tss/` → Task state (if applicable)
- `hal/cpuid/` → CPU feature detection
- `hal/msrs/` → Model-specific registers

### 3. Memory Management

- `mm/pmm/` → Physical allocator (bitmap is generic)
- `mm/vmm/` → Page tables (architecture-specific format)
- `mm/heap/` → Kernel heap (mostly generic)

### 4. Drivers

Port or replace:
- `drivers/pic/` → Interrupt controller (ARM GIC, RISC-V PLIC, etc.)
- `drivers/pit/` → System timer
- `drivers/serial/` → UART (ARM PL011, RISC-V NS16550A, etc.)

### 5. Inline Assembly

Replace `gOS/types.h` inline assembly wrappers:
- `inb`/`outb` → MMIO read/write for non-x86
- `read_cr3`/`write_cr3` → Architecture MMU registers
- `rdmsr`/`wrmsr` → Architecture-specific config registers
- `cli`/`sti` → Interrupt enable/disable

### 6. Build System

Update `Makefile`:
- Cross-compiler prefix (`x86_64-elf-gcc` → `aarch64-elf-gcc`)
- Architecture flags (`-m64` → `-march=armv8-a`)
- Linker script (page size, virtual base)

### 7. QEMU Testing

Update `scripts/run.sh`:
```bash
qemu-system-aarch64 -M virt -cpu cortex-a72 ...
```

## Reference Architectures

| Architecture | Status |
|-------------|--------|
| x86-64 | Supported |
| AArch64 | Planned |
| RISC-V | Planned |
| i386 | Dropped (legacy) |

## Reference

- https://wiki.osdev.org/Portability
- https://wiki.osdev.org/ARM64_Bare_Bones
- https://wiki.osdev.org/RISC-V_Bare_Bones

# Hacking on gOS

## Quick Start

```bash
# Build cross-compiler (one-time)
./scripts/toolchain.sh

# Build kernel
make

# Run in QEMU
make run

# Debug with GDB
make debug
```

## Adding a New Driver

1. Create directory `drivers/[device]/`
2. Write `[device].h` with public API
3. Write `[device].c` with implementation
4. Write `[device]_irq.c` if interrupt-driven
5. Add to `Makefile` `C_SOURCES`
6. Call `[device]_init()` from `kernel_early_init()`

## Adding a New System Call

1. Add syscall number to `sys/syscall.h`
2. Implement handler in `sys/syscall.c`
3. Register with `syscall_register()`

## Debugging Triple Faults

1. Check IDT is loaded before `sti`
2. Verify PIC remap happened before unmasking IRQs
3. Ensure `io_wait()` after legacy port writes
4. Check stack is not overflowing into kernel data
5. Use QEMU `-d int,cpu_reset` to see the fault vector

## Kernel Memory Map During Boot

```
0x0000000000000000  ->  Identity mapped low memory
0x0000000000100000  ->  Kernel physical base (1MB)
0xFFFFFFFF80000000  ->  Kernel virtual base (higher half)
0xFFFFFFFF80200000  ->  Kernel heap start
0xFFFFFFFF81000000  ->  VMM scratch area
```

## Reference

- https://wiki.osdev.org/Debugging
- https://wiki.osdev.org/QEMU

# gOS Architecture Overview

## System Layers

```
User Space (future)
  |
  v
System Call Interface
  |
  v
+---------------------------+
| Kernel Space (Ring 0)     |
|  +---------------------+  |
|  | VFS / Filesystem    |  |
|  +---------------------+  |
|  +---------------------+  |
|  | Process / Scheduler |  |
|  +---------------------+  |
|  +---------------------+  |
|  | Memory Manager      |  |
|  +---------------------+  |
|  +---------------------+  |
|  | Driver Subsystem    |  |
|  | PIC PIT KB VGA ...|  |
|  +---------------------+  |
+---------------------------+
  |
  v
+---------------------------+
| Hardware Abstraction (HAL)|
| GDT IDT TSS CPUID MSR   |
+---------------------------+
  |
  v
GRUB2 / Multiboot2
```

## Boot Flow

1. **GRUB2** loads kernel via Multiboot2 protocol
2. **boot.asm** — 32-bit protected mode entry, sets up paging
3. **long_mode.asm** — enters 64-bit long mode, loads 64-bit GDT
4. **kernel_early_init()** — initializes serial, memory, HAL, drivers
5. **kernel_main()** — enters idle loop with interrupts enabled

## Memory Layout (Higher Half Kernel)

| Virtual Address | Physical | Purpose |
|-----------------|----------|---------|
| 0x0000000000000000 | identity | User space (future) |
| 0xFFFFFFFF80000000 | 0x100000 | Kernel code/data (higher half) |
| 0xFFFFFFFF80200000 | heap | Kernel heap (kmalloc/kfree) |
| 0xFFFFFFFF80400000 | heap | Kernel stack area |
| 0xFFFFFFFF80800000 | PMM | Page tables, bitmaps |

## Interrupt Layout

| Vector | Type |
|--------|------|
| 0-31 | CPU Exceptions |
| 32-47 | Remapped PIC IRQs |
| 80, 128 | Legacy syscall entry |

## Reference

- https://wiki.osdev.org/Bare_Bones
- https://wiki.osdev.org/Setting_Up_Long_Mode

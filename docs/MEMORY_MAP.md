# gOS Kernel Memory Map

## Physical Memory

| Region | Start | End | Purpose |
|--------|-------|-----|---------|
| Reserved | 0x00000 | 0x9FFFF | BIOS, VGA, ISA |
| Kernel | 0x100000 | variable | Kernel image (.text, .data, .bss) |
| Page Tables | after kernel | variable | PML4, PDPT, PD, PT pages |
| PMM Bitmap | after page tables | variable | Physical page allocation tracking |
| Heap | 0x10400000+ | 0x14400000 | kmalloc/kfree area (64MB max) |
| Available | rest | 0x10000000 (256MB) | Free for allocation |

## Page Table Hierarchy

```
PML4[0] -> PDPT_LOW -> PD -> PT (identity map first 1GB)
PML4[511] -> PDPT_HIGH -> PD -> PT (higher half at 0xFFFFFFFF80000000)
```

## Virtual Address Space

| Range | Usage |
|-------|-------|
| 0x0000000000000000 - 0x00007FFFFFFFFFFF | User space (future) |
| 0xFFFF800000000000 - 0xFFFFFFFF7FFFFFFF | Kernel physical map (future) |
| 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF | Kernel code/data |

## GDT Layout

| Selector | Index | Type |
|----------|-------|------|
| 0x00 | 0 | Null |
| 0x08 | 1 | 64-bit Kernel Code |
| 0x10 | 2 | 64-bit Kernel Data |
| 0x18 | 3 | 32-bit User Code |
| 0x20 | 4 | 64-bit User Data |
| 0x28 | 5 | 64-bit User Code |
| 0x30 | 6 | TSS (low) |
| 0x38 | 7 | TSS (high) |

## Reference

- https://wiki.osdev.org/Paging
- https://wiki.osdev.org/Detecting_Memory_(x86)
- https://wiki.osdev.org/GDT

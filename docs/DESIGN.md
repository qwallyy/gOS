# gOS Design Decisions

## Higher Half Kernel

The kernel is loaded at physical address 0x100000 (1MB) and mapped at virtual
address 0xFFFFFFFF80000000. This leaves the lower half of the address space
entirely for user processes and simplifies memory management.

## No Red Zone

`-mno-red-zone` disables the 128-byte red zone below RSP. Without this, an
interrupt firing mid-function would overwrite the red zone, corrupting the
function's local variables.

## Separate IRQ Handlers

Some drivers split their core logic (`device.c`) from their interrupt handler
(`device_irq.c`). This allows selective compilation and cleaner separation of
concerns.

## Stub-First Development

Many subsystems (TCP, USB host controllers, AHCI ports) are implemented as
stubs that detect hardware and log presence. Full functionality is layered on
top once the detection path is verified.

## Memory Allocation Strategy

- PMM: bitmap allocator for physical pages (up to 64GB)
- VMM: 4-level page tables, identity map first 1GB + higher half kernel
- Heap: bump allocator with free list, coalescing on `kfree`
- Slab: object caches for fixed-size allocations (future optimization)

## Interrupt Handling

- PIC remapped to vectors 32-47
- IST used for NMI (#2), Double Fault (#8), Machine Check (#18)
- Common C handler receives a uniform `interrupt_frame_t *`
- EOI sent before returning from every IRQ handler

## Filesystem Stack

```
VFS -> TMPFS/DEVFS/FAT32/EXT2/ISO9660
        |
        v
    Block Device Layer
        |
        v
    ATA PIO / AHCI / Floppy
```

## Network Stack

```
Socket API (stub)
  |
  v
TCP / UDP / ICMP
  |
  v
IP / ARP / NDP
  |
  v
Ethernet
  |
  v
NIC (E1000 / RTL8139)
```

## Reference

- https://wiki.osdev.org/Higher_Half_Kernel
- https://wiki.osdev.org/Red_Zone
- https://wiki.osdev.org/Memory_Allocation

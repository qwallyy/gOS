# gOS Changelog

## [0.1.0] — Development

### Added
- Multiboot2 boot header with GRUB2 support
- 32-bit protected mode to 64-bit long mode transition
- Flat memory model GDT with kernel/user segments
- IDT with 256 entries, IST for NMI/double fault/machine check
- ISR stubs for all CPU exceptions (0-31)
- IRQ stubs for PIC (0-15)
- PIC remapping to vectors 32-47
- PIT timer at 100Hz
- Serial port (COM1, UART 16550A) driver
- VGA text mode terminal (80x25)
- PS/2 keyboard driver with scancode-to-ASCII translation
- PS/2 mouse driver
- Real-time clock (RTC) driver
- PCI bus enumeration
- ATA PIO mode driver
- AHCI (SATA) controller detection
- USB host controller detection (UHCI/OHCI/EHCI/XHCI)
- Intel E1000 NIC driver stub
- Realtek RTL8139 NIC driver stub
- Physical memory manager (bitmap-based, up to 64GB)
- Virtual memory manager (4-level paging, x86-64)
- Kernel heap (kmalloc/kfree with split/merge)
- VFS abstraction layer
- FAT32 filesystem driver stub
- Block device layer
- Round-robin process scheduler
- Process creation/destruction/yield
- System call table (legacy INT-based + SYSCALL/SYSRET MSRs)
- Ethernet frame handling
- ARP cache
- IPv4 packet parsing
- ICMP echo detection
- UDP packet parsing
- TCP stub
- ACPI RSDP/RSDT parsing
- Local APIC detection
- I/O APIC detection
- Legacy DMA controller interface
- Timekeeping (PIT-based milliseconds)
- Error code definitions (POSIX-style)
- String, stdio, stdlib, assert libraries
- CRC32 checksum
- FNV-1a hash
- Bit operations (clz, ctz, popcount)
- Quicksort, heapsort, binary search
- Intrusive linked list
- Spinlock (with interrupt-safe variant)
- Ring buffer
- Generic FIFO queue
- Slab allocator
- Security context stub
- Power management (ACPI/virtual shutdown, reboot)
- IPC message queues
- Unit tests for string, PMM, heap, sort
- Integration tests for boot invariants
- Build scripts and documentation

### Known Issues
- FAT32 read/write not fully implemented
- Network send requires IP layer completion
- SMP not yet implemented
- User mode (Ring 3) not yet entered

# gOS Development TODO

## Stage 1 — Bare Bones (COMPLETE)
- [x] Multiboot2 header
- [x] 32-bit to 64-bit long mode transition
- [x] Serial output on COM1
- [x] VGA text mode terminal

## Stage 2 — GDT (COMPLETE)
- [x] Flat memory model GDT
- [x] Kernel code/data segments
- [x] User code/data segments (for future)
- [x] TSS descriptor

## Stage 3 — IDT & ISR (COMPLETE)
- [x] 256-entry IDT
- [x] CPU exception handlers (0-31)
- [x] IST for NMI, Double Fault, Machine Check
- [x] ISR stub assembly macros

## Stage 4 — PIC (COMPLETE)
- [x] 8259 PIC remap to vectors 32-47
- [x] IRQ stub registration
- [x] Spurious IRQ detection

## Stage 5 — Memory Management (COMPLETE)
- [x] Bitmap-based PMM
- [x] 4-level paging VMM
- [x] Kernel heap (kmalloc/kfree)
- [x] Identity + higher half mappings

## Stage 6 — Timer (COMPLETE)
- [x] PIT at 100Hz
- [x] System tick counter
- [x] Sleep/delay functions

## Stage 7 — Keyboard (COMPLETE)
- [x] PS/2 scancode set 1
- [x] Scancode to ASCII translation
- [x] Modifier key tracking
- [x] Keyboard buffer

## Stage 8 — VGA / Framebuffer (PARTIAL)
- [x] VGA text mode
- [x] Cursor control
- [x] Scrolling
- [ ] VESA linear framebuffer (detected, basic pixel ops)

## Stage 9 — PCI (COMPLETE)
- [x] PCI bus enumeration
- [x] BAR reading
- [x] Device class detection

## Stage 10 — ATA/AHCI (PARTIAL)
- [x] ATA PIO read/write
- [x] AHCI controller detection
- [ ] AHCI port command queue

## Stage 11 — Filesystem (PARTIAL)
- [x] VFS abstraction
- [x] Block device layer
- [x] FAT32 mount / parse BPB
- [ ] FAT32 read/write/cluster chain
- [ ] Directory traversal

## Stage 12 — Process & Scheduler (PARTIAL)
- [x] Task structure
- [x] Round-robin scheduler
- [x] Context switch (assembly)
- [ ] Full user mode (Ring 3) entry
- [ ] ELF loading

## Stage 13 — System Calls (PARTIAL)
- [x] INT 0x50 legacy entry
- [x] SYSCALL/SYSRET MSRs
- [x] Basic syscall table
- [ ] Full user syscall ABI

## Stage 14 — USB (DETECTION ONLY)
- [x] USB controller detection (UHCI/OHCI/EHCI/XHCI)
- [ ] Host controller init
- [ ] Device enumeration

## Stage 15 — Network (PARTIAL)
- [x] NIC abstraction (E1000, RTL8139 detection)
- [x] Ethernet frame parsing
- [x] ARP cache
- [x] IPv4 parsing
- [ ] Full IP send path
- [ ] TCP state machine

## Future Work
- [ ] SMP / AP startup
- [ ] APIC timer as primary clock source
- [ ] HPET calibration and use
- [ ] ACPI shutdown/reboot
- [ ] User mode Ring 3 processes
- [ ] ELF executable loader
- [ ] Initrd / ramdisk support
- [ ] POSIX signal delivery
- [ ] Full TCP stack
- [ ] DHCP client
- [ ] Serial console shell
- [ ] Kernel module loading

# gOS — Custom x86-64 Operating System

> A minimal operating system kernel for x86-64, built from scratch following [OSDev.org](https://osdev.org) best practices.

## Architecture

- **Target:** x86-64 (AMD64)
- **Boot:** Multiboot2 via GRUB 2
- **Mode:** Protected Mode -> Long Mode transition
- **Language:** C11 + x86 NASM Assembly

## Directory Layout

```
gOS/
  boot/          Multiboot2 header and boot sequence
  arch/x86_64/   Architecture-specific assembly (long mode, ISR stubs)
  kernel/        Kernel main entry, panic handler
  hal/           Hardware Abstraction Layer (GDT, IDT, TSS, CPUID, MSRs)
  drivers/       Device drivers (PIC, PIT, PS/2, VGA, Serial, RTC, PCI, ATA, AHCI, USB, NIC, E1000, RTL8139)
  mm/            Memory management (PMM, VMM, Heap)
  fs/            Filesystem layer (VFS, FAT32, Block)
  proc/          Process and scheduler
  sys/           System calls
  net/           Network stack (Ethernet, ARP, IP)
  lib/           Kernel string, stdio, stdlib, assert
  acpi/          ACPI table parsing
  dma/           Legacy DMA controller
  time/          Timekeeping
  errno/         Error codes
  include/gOS/   Global kernel headers
  linker/        Kernel linker script
  grub/          GRUB2 boot configuration
  Makefile       Top-level build system
```

## Building

Requires a cross-compiler toolchain:

```bash
# Install cross-compiler (x86_64-elf-gcc, x86_64-elf-ld, nasm)
# Reference: https://wiki.osdev.org/GCC_Cross-Compiler

make          # Build kernel.elf
make iso      # Build bootable ISO (gOS.iso)
make run      # Run in QEMU
make debug    # Run with GDB stub
```

## QEMU Test Command

```bash
qemu-system-x86_64 \
  -cdrom gOS.iso \
  -m 256M \
  -serial stdio \
  -d int,cpu_reset \
  -no-reboot \
  -no-shutdown
```

## Debugging Triple Faults

```bash
qemu-system-x86_64 -cdrom gOS.iso -s -S -no-reboot &
gdb -ex "target remote :1234" -ex "set arch i386:x86-64" build/kernel.elf
```

## References

All implementation decisions are grounded in OSDev.org articles:

- [Bare Bones](https://wiki.osdev.org/Bare_Bones)
- [GDT](https://wiki.osdev.org/GDT) / [GDT Tutorial](https://wiki.osdev.org/GDT_Tutorial)
- [IDT](https://wiki.osdev.org/IDT) / [ISR](https://wiki.osdev.org/ISR)
- [8259 PIC](https://wiki.osdev.org/8259_PIC)
- [Paging](https://wiki.osdev.org/Paging)
- [ATA PIO Mode](https://wiki.osdev.org/ATA_PIO_Mode)
- [AHCI](https://wiki.osdev.org/AHCI)
- [FAT](https://wiki.osdev.org/FAT)
- [VFS](https://wiki.osdev.org/VFS)
- [Processes and Threads](https://wiki.osdev.org/Processes_and_Threads)
- [System Calls](https://wiki.osdev.org/System_Calls)
- [Network Stack](https://wiki.osdev.org/Network_Stack)
- [ACPI](https://wiki.osdev.org/ACPI)

## License

Developed for educational purposes as part of the gOS project.

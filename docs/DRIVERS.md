# gOS Driver Development Guide

## Driver Protocol

Every driver must follow the protocol defined in task.md:

1. **Identify Hardware Spec Source** — locate the OSDev wiki article
2. **I/O Port / MMIO Safety** — use `inb()`/`outb()` wrappers, insert `io_wait()` after legacy port writes
3. **Interrupt-Driven vs Polling** — prefer interrupts, send EOI after every IRQ handler
4. **Error Handling** — timeout loops on every port read/write, log errors via `klog()`, never panic on non-critical driver errors

## File Layout

```
drivers/
├── [device]/
│   ├── [device].h        # Public API: init, read, write, ioctl-like functions
│   ├── [device].c        # Implementation
│   └── [device]_irq.c    # IRQ handler (if applicable)
```

## I/O Port Reference

| Device | Base Port | Notes |
|--------|-----------|-------|
| PIC Master | 0x20, 0x21 | Must remap before enabling IRQs |
| PIC Slave | 0xA0, 0xA1 | Cascade on IRQ2 |
| PIT | 0x40-0x43 | Channel 0 = system timer |
| PS/2 Keyboard | 0x60, 0x64 | Uses IRQ1 |
| PS/2 Controller | 0x60, 0x64 | Status + command ports |
| Serial COM1 | 0x3F8 | UART 16550A |
| Serial COM2 | 0x2F8 | UART 16550A |
| RTC | 0x70, 0x71 | CMOS index + data |
| VGA | 0x3D4, 0x3D5 | CRT controller |
| PCI Config | 0xCF8, 0xCFC | Configuration access |
| ATA Primary | 0x1F0-0x1F7 | PIO mode |
| ATA Secondary | 0x170-0x177 | PIO mode |
| DMA Controller | 0x00-0x0F | Legacy 8237 |
| DMA Page Regs | 0x80-0x8F | Page registers |

## Common PCI Classes

| Class | Subclass | Description |
|-------|----------|-------------|
| 0x01 | 0x01 | IDE Controller |
| 0x01 | 0x06 | SATA AHCI |
| 0x02 | 0x00 | Ethernet |
| 0x0C | 0x03 | USB Controller |
| 0x0C | 0x03 prog 0x00 | UHCI |
| 0x0C | 0x03 prog 0x10 | OHCI |
| 0x0C | 0x03 prog 0x20 | EHCI |
| 0x0C | 0x03 prog 0x30 | XHCI |

## Error Codes

| Code | Value | Meaning |
|------|-------|---------|
| ENODEV | 19 | No such device |
| EIO | 5 | I/O error |
| ENOMEM | 12 | Out of memory |
| ETIMEOUT | 116 | Operation timed out |
| EINVAL | 22 | Invalid argument |

## Testing Drivers

```bash
# QEMU with specific devices
qemu-system-x86_64 -cdrom gOS.iso -serial stdio -device e1000 -device piix3-usb-uhci
```

## Reference

- https://wiki.osdev.org/PCI
- https://wiki.osdev.org/8259_PIC
- https://wiki.osdev.org/ATA_PIO_Mode
- https://wiki.osdev.org/AHCI
- https://wiki.osdev.org/Intel_8254x
- https://wiki.osdev.org/RTL8139

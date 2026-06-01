# gOS I/O Port Reference

## Primary System Ports

| Port Range | Device | Notes |
|-----------|--------|-------|
| 0x20, 0x21 | PIC Master | Remapped to vectors 32-39 |
| 0xA0, 0xA1 | PIC Slave | Remapped to vectors 40-47 |
| 0x40-0x43 | PIT | Channel 0 = system timer |
| 0x60, 0x64 | PS/2 | Keyboard + controller |
| 0x70, 0x71 | RTC/CMOS | Index + data ports |
| 0x80 | POST / I/O Delay | `io_wait()` uses this |
| 0x3F8-0x3FF | COM1 | Serial UART 16550A |
| 0x2F8-0x2FF | COM2 | Serial UART 16550A |
| 0x3E8-0x3EF | COM3 | Serial UART 16550A |
| 0x2E8-0x2EF | COM4 | Serial UART 16550A |
| 0x3D4, 0x3D5 | VGA CRT | CRTC controller |
| 0x3B4, 0x3B5 | MDA CRT | Mono CRTC (fallback) |
| 0x3C0-0x3CF | VGA Attribute | DAC, sequencer |
| 0x3D8-0x3DF | CGA Ports | Legacy |
| 0x1F0-0x1F7 | ATA Primary | PIO data + control |
| 0x170-0x177 | ATA Secondary | PIO data + control |
| 0x376 | ATA Primary Ctrl | Alternate status |
| 0x3F0-0x3F7 | Floppy | FDC primary |
| 0x370-0x377 | Floppy | FDC secondary |
| 0x378-0x37F | LPT1 | Parallel port |
| 0x278-0x27F | LPT2 | Parallel port |
| 0x3BC-0x3BE | LPT3 | Parallel port (often on video card) |
| 0xCF8 | PCI Config Address | 32-bit |
| 0xCFC | PCI Config Data | 32-bit |
| 0x501 | QEMU Debug Exit | `outb(0x501, 0)` triggers shutdown |
| 0x00-0x0F | DMA 1 | Channels 0-3 |
| 0x80-0x8F | DMA Page Regs | Page registers for channels |

## PIC Register Map

### Master (0x20 = command, 0x21 = data)

| Bit | Name | Description |
|-----|------|-------------|
| 0 | M0 | Mask IRQ0 |
| 1 | M1 | Mask IRQ1 |
| ... | ... | ... |
| 7 | M7 | Mask IRQ7 |

### Slave (0xA0 = command, 0xA1 = data)

Same layout for IRQ8-IRQ15.

## PIT Register Map

| Port | Function |
|------|----------|
| 0x40 | Channel 0 counter (system timer) |
| 0x41 | Channel 1 (legacy DRAM refresh) |
| 0x42 | Channel 2 (PC speaker) |
| 0x43 | Mode/command register |

## Reference

- https://wiki.osdev.org/I/O_Ports
- https://wiki.osdev.org/8259_PIC
- https://wiki.osdev.org/Programmable_Interval_Timer

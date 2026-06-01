# gOS Interrupt Reference

## CPU Exception Vectors (0-31)

| Vector | Name | Mnemonic | Has Error Code | IST Used |
|--------|------|----------|---------------|----------|
| 0 | Divide Error | #DE | No | No |
| 1 | Debug | #DB | No | No |
| 2 | Non-Maskable Interrupt | #NMI | No | IST1 |
| 3 | Breakpoint | #BP | No | No |
| 4 | Overflow | #OF | No | No |
| 5 | Bound Range | #BR | No | No |
| 6 | Invalid Opcode | #UD | No | No |
| 7 | Device Not Available | #NM | No | No |
| 8 | Double Fault | #DF | Yes | IST2 |
| 9 | Coprocessor Segment Overrun | — | No | No |
| 10 | Invalid TSS | #TS | Yes | No |
| 11 | Segment Not Present | #NP | Yes | No |
| 12 | Stack-Segment Fault | #SS | Yes | No |
| 13 | General Protection Fault | #GP | Yes | No |
| 14 | Page Fault | #PF | Yes | No |
| 16 | x87 FPU Error | #MF | No | No |
| 17 | Alignment Check | #AC | Yes | No |
| 18 | Machine Check | #MC | No | IST3 |
| 19 | SIMD Exception | #XF | No | No |
| 20 | Virtualization | #VE | No | No |
| 21 | Control Protection | #CP | Yes | No |
| 29 | Hypervisor Injection | #HV | No | No |
| 30 | VMM Communication | #VC | Yes | No |

## IRQ Mapping (PIC Remapped)

| IRQ | Vector | Hardware |
|-----|--------|----------|
| 0 | 32 | PIT Timer |
| 1 | 33 | PS/2 Keyboard |
| 2 | 34 | Cascade (PIC2) |
| 3 | 35 | COM2 / COM4 |
| 4 | 36 | COM1 / COM3 |
| 5 | 37 | LPT2 |
| 6 | 38 | Floppy Disk |
| 7 | 37 | LPT1 / Spurious |
| 8 | 40 | RTC |
| 9 | 41 | Free (legacy) |
| 10 | 42 | Free |
| 11 | 43 | Free |
| 12 | 44 | PS/2 Mouse |
| 13 | 45 | FPU / Coprocessor |
| 14 | 46 | Primary ATA |
| 15 | 47 | Secondary ATA / Spurious |

## IST Stack Assignment

| IST | Exception | Stack Size |
|-----|-----------|-----------|
| IST1 | NMI (#2) | 8KB |
| IST2 | Double Fault (#8) | 8KB |
| IST3 | Machine Check (#18) | 8KB |

## Spurious IRQ Detection

Spurious IRQ7 and IRQ15 occur when the PIC receives an interrupt but the
ISR register shows no pending interrupt. We check the ISR before sending EOI.

## Reference

- https://wiki.osdev.org/Exceptions
- https://wiki.osdev.org/8259_PIC
- https://wiki.osdev.org/ISR

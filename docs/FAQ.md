# gOS Frequently Asked Questions

## Why does gOS need a cross-compiler?

Your host GCC generates binaries for your host OS (Linux/macOS), which link
against the host libc. The gOS kernel is a bare-metal program with no libc.
A cross-compiler generates ELF objects for x86-64 without assuming any OS.

Reference: https://wiki.osdev.org/Why_do_I_need_a_Cross_Compiler%3F

## Why is the kernel mapped at 0xFFFFFFFF80000000?

This is the "higher half" design. In x86-64 canonical addressing, the upper
half of the address space (0xFFFF800000000000+) is reserved for the kernel,
leaving the lower half for user processes. We map the kernel at a known
offset within that upper half.

## Why does the IDT use IST for NMI and Double Fault?

If a stack overflow or corruption causes a fault, the CPU tries to push the
exception frame onto the same corrupt stack — resulting in a double fault.
Using an IST (Interrupt Stack Table) gives each critical exception a fresh,
dedicated stack that cannot overflow.

## Why disable the red zone?

The x86-64 ABI defines a 128-byte red zone below RSP that functions can use
without adjusting the stack pointer. If an interrupt fires in the middle of a
function using the red zone, the ISR will overwrite it. `-mno-red-zone` disables
this optimization in kernel code.

## How do I debug a triple fault?

Run QEMU with `-d int,cpu_reset` and check the serial output. Use GDB with
`-s -S` flags. The most common causes are:
- Invalid IDT/PIC state when `sti` is called
- Stack overflow in an ISR
- Page fault in a page fault handler (recursive)
- Wrong segment selector in an IDT gate

## Can I run gOS on real hardware?

In theory, yes — it targets standard x86-64 PC hardware. However, real hardware
has many more device variants than QEMU emulates. Drivers for your specific
storage controller, NIC, and ACPI tables may need additional work.

## What filesystems are supported?

Currently: VFS abstraction with FAT32 read stubs. Full FAT32 read/write and
directory traversal is planned.

## Is SMP (multi-core) supported?

Not yet. The SMP stubs are in place, but AP (Application Processor) startup,
IPI routing, and per-CPU scheduler state are future work.

## How do I add a new driver?

1. Create `drivers/[device]/[device].h` and `.c`
2. Add it to `Makefile` `C_SOURCES`
3. Call its `_init()` function from `kernel_early_init()` or `kernel_main()`
4. Follow the driver protocol in `docs/DRIVERS.md`

## What is the license?

gOS is developed for educational purposes. See the repository for license terms.

# Contributing to gOS

## Development Workflow

1. **File a bug or feature request** describing the issue
2. **Discuss design** referencing OSDev wiki articles
3. **Implement** following the driver protocol in `docs/DRIVERS.md`
4. **Test** using QEMU with `-d int,cpu_reset`
5. **Submit** with clear commit messages

## Code Style

- All magic numbers must have named `#define` constants
- All I/O port operations use `inb()`/`outb()` wrappers with `io_wait()` where required
- Functions must have a brief comment citing the OSDev article
- Assembly inline uses `volatile`
- Kernel panic prints file, line, and reason before halting

## Checklist Before Submitting

- [ ] Cross-compiler used (not host GCC)
- [ ] QEMU boots without triple fault
- [ ] Serial output is clean and informative
- [ ] No `malloc`/`free` from libc in kernel code
- [ ] `sti` not called before IDT/PIC fully initialized
- [ ] EOI sent after every IRQ handler

## Reference

- https://wiki.osdev.org/Bare_Bones
- https://wiki.osdev.org/Why_do_I_need_a_Cross_Compiler%3F

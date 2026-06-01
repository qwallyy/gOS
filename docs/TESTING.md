# gOS Testing Guide

## Unit Tests

Located in `tests/unit/`. Each module has its own test file.

### Running Unit Tests

Unit tests run automatically during kernel boot in debug builds:

```
kernel_early_init() -> run_all_unit_tests()
```

### Available Unit Tests

| File | Coverage |
|------|----------|
| test_string.c | strlen, strcmp, strcpy, memcpy, memset |
| test_pmm.c | page alloc/free, multi-page alloc |
| test_heap.c | kmalloc/kfree, kzalloc, krealloc |
| test_sort.c | qsort, heapsort, bsearch |
| test_crc32.c | CRC32 computation |
| test_bitops.c | clz, ctz, popcount, rol, ror |
| test_math.c | abs, min/max, align, div_round_up |
| test_queue.c | enqueue, dequeue, clear |
| test_list.c | intrusive list insert/remove/iterate |
| test_ring_buffer.c | push/pop, wrap-around |
| test_slab.c | slab cache create/alloc/destroy |
| test_hash.c | djb2, fnv1a |

## Integration Tests

Located in `tests/integration/`. These test subsystem interactions.

| File | Coverage |
|------|----------|
| test_boot.c | GDT/IDT loaded, paging active, long mode |
| test_interrupts.c | PIC remap, mask/unmask, ISR read |
| test_memory.c | PMM->VMM->heap alloc chain |
| test_drivers.c | Serial, VGA, PIT, keyboard, RTC, PCI |
| test_scheduler.c | Process creation, tick advance |
| test_network.c | Ethernet, ARP, IP, ICMP, UDP, TCP init |
| test_vfs.c | VFS, DEVFS, TMPFS mount and ops |

## Stress Tests

Located in `tests/stress/`.

| File | Coverage |
|------|----------|
| test_memory_stress.c | 256 page allocs, 512 heap allocs |
| test_interrupt_stress.c | Timer stability, 1000 yields |

## QEMU Debug Flags

```bash
qemu-system-x86_64 -d int,cpu_reset,guest_errors -no-reboot
```

## GDB Debugging

```bash
make debug
# In another terminal:
gdb -ex "target remote :1234" -ex "set arch i386:x86-64" build/kernel.elf
```

## Common Failure Modes

| Symptom | Likely Cause |
|---------|-------------|
| Triple fault immediately | IDT not loaded before `sti` |
| No serial output | COM1 not initialized or wrong port |
| Page fault in boot | Page tables not identity-mapping low memory |
| Timer not firing | PIC not remapped or IRQ0 not unmasked |
| Keyboard no response | IRQ1 masked or PS/2 controller not initialized |

## Reference

- https://wiki.osdev.org/Debugging
- https://wiki.osdev.org/QEMU

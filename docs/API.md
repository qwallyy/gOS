# gOS Kernel API Reference

## Kernel Logging (`gOS/klog.h`)

```c
void klog(int level, const char *file, int line, const char *fmt, ...);
void klog_init(void);
void klog_hexdump(const void *data, size_t len, uintptr_t base_addr);
```

Levels: `KLOG_EMERG` (0) through `KLOG_DEBUG` (7).

## Memory Management

### Physical Memory Manager (`mm/pmm/pmm.h`)

```c
void  *pmm_alloc_page(void);
void  *pmm_alloc_pages(size_t count);
void   pmm_free_page(void *page);
void   pmm_free_pages(void *pages, size_t count);
uintptr_t pmm_get_total_memory(void);
uintptr_t pmm_get_free_memory(void);
```

### Virtual Memory Manager (`mm/vmm/vmm.h`)

```c
void vmm_map_page(uintptr_t virt, uintptr_t phys, uint64_t flags);
void vmm_unmap_page(uintptr_t virt);
bool vmm_is_mapped(uintptr_t virt);
uintptr_t vmm_get_phys(uintptr_t virt);
```

Page flags: `VMM_PRESENT`, `VMM_WRITABLE`, `VMM_USER`, `VMM_NOCACHE`, `VMM_NX`.

### Heap (`mm/heap/heap.h`)

```c
void *kmalloc(size_t size);
void *kzalloc(size_t size);
void *krealloc(void *ptr, size_t new_size);
void  kfree(void *ptr);
size_t ksize(void *ptr);
```

## Process & Scheduler (`proc/proc.h`, `proc/scheduler.h`)

```c
process_t *proc_create(const char *name, void (*entry)(void));
void        proc_destroy(process_t *proc);
void        proc_yield(void);
void        proc_exit(int code);
void        proc_sleep(uint64_t ms);
process_t  *proc_get_current(void);
```

## System Calls (`sys/syscall.h`)

| Number | Name | Args |
|--------|------|------|
| 0 | `sys_exit` | `code` |
| 1 | `sys_fork` | — |
| 2 | `sys_read` | `fd, buf, count` |
| 3 | `sys_write` | `fd, buf, count` |
| 4 | `sys_open` | `path, flags` |
| 5 | `sys_close` | `fd` |
| 6 | `sys_sleep` | `ms` |
| 7 | `sys_getpid` | — |
| 8 | `sys_yield` | — |
| 9 | `sys_malloc` | `size` |
| 10 | `sys_free` | `ptr` |

## I/O Port (`gOS/types.h`)

```c
void    outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void    outw(uint16_t port, uint16_t val);
uint16_t inw(uint16_t port);
void    outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);
void    io_wait(void);
```

## Interrupt (`gOS/types.h`)

```c
void cli(void);   /* Disable interrupts */
void sti(void);   /* Enable interrupts */
void hlt(void);   /* Halt CPU until next interrupt */
```

## Panic (`kernel/panic.c`)

```c
NORETURN void kernel_panic(const char *file, int line, const char *reason);
```

## Reference

- https://wiki.osdev.org/Bare_Bones
- https://wiki.osdev.org/Page_Frame_Allocation
- https://wiki.osdev.org/Scheduling_Algorithms

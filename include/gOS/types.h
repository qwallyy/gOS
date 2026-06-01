/* gOS Kernel — Common Types and Inline I/O
 * Reference: https://wiki.osdev.org/Inline_Assembly/Examples
 *
 * Provides standard integer types and I/O port wrappers used throughout
 * the kernel. All drivers must use these wrappers rather than raw inline
 * assembly to ensure consistent compiler barriers and I/O wait semantics.
 */

#ifndef GOS_TYPES_H
#define GOS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* I/O Port Output — write 8-bit value to port */
/* Reference: https://wiki.osdev.org/Inline_Assembly/Examples */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

/* I/O Port Output — write 16-bit value to port */
static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

/* I/O Port Output — write 32-bit value to port */
static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

/* I/O Port Input — read 8-bit value from port */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/* I/O Port Input — read 16-bit value from port */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/* I/O Port Input — read 32-bit value from port */
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

/* I/O Wait — required for legacy ISA bus devices */
/* Reference: https://wiki.osdev.org/Inline_Assembly/Examples */
/* Writing to port 0x80 is a safe no-op that introduces an I/O delay */
static inline void io_wait(void) {
    outb(0x80, 0);
}

/* Memory-mapped I/O helpers */
static inline void mmio_write8(uintptr_t addr, uint8_t val) {
    volatile uint8_t *ptr = (volatile uint8_t *)addr;
    *ptr = val;
}

static inline void mmio_write16(uintptr_t addr, uint16_t val) {
    volatile uint16_t *ptr = (volatile uint16_t *)addr;
    *ptr = val;
}

static inline void mmio_write32(uintptr_t addr, uint32_t val) {
    volatile uint32_t *ptr = (volatile uint32_t *)addr;
    *ptr = val;
}

static inline void mmio_write64(uintptr_t addr, uint64_t val) {
    volatile uint64_t *ptr = (volatile uint64_t *)addr;
    *ptr = val;
}

static inline uint8_t mmio_read8(uintptr_t addr) {
    volatile uint8_t *ptr = (volatile uint8_t *)addr;
    return *ptr;
}

static inline uint16_t mmio_read16(uintptr_t addr) {
    volatile uint16_t *ptr = (volatile uint16_t *)addr;
    return *ptr;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
    volatile uint32_t *ptr = (volatile uint32_t *)addr;
    return *ptr;
}

static inline uint64_t mmio_read64(uintptr_t addr) {
    volatile uint64_t *ptr = (volatile uint64_t *)addr;
    return *ptr;
}

/* Halt the CPU until the next interrupt */
static inline void hlt(void) {
    __asm__ volatile ("hlt");
}

/* Disable interrupts */
static inline void cli(void) {
    __asm__ volatile ("cli");
}

/* Enable interrupts */
static inline void sti(void) {
    __asm__ volatile ("sti");
}

/* Read CPU timestamp counter (64-bit) */
static inline uint64_t rdtsc(void) {
    uint32_t low, high;
    __asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
    return ((uint64_t)high << 32) | low;
}

/* Read CR3 (page table base physical address) */
static inline uint64_t read_cr3(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(val));
    return val;
}

/* Write CR3 */
static inline void write_cr3(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(val));
}

/* Read CR0 */
static inline uint64_t read_cr0(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(val));
    return val;
}

/* Write CR0 */
static inline void write_cr0(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr0" : : "r"(val));
}

/* Read CR2 (page fault linear address) */
static inline uint64_t read_cr2(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(val));
    return val;
}

/* Read CR4 */
static inline uint64_t read_cr4(void) {
    uint64_t val;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(val));
    return val;
}

/* Write CR4 */
static inline void write_cr4(uint64_t val) {
    __asm__ volatile ("mov %0, %%cr4" : : "r"(val));
}

/* Read RFLAGS */
static inline uint64_t read_rflags(void) {
    uint64_t val;
    __asm__ volatile ("pushfq; popq %0" : "=r"(val));
    return val;
}

/* Get current stack pointer */
static inline uintptr_t read_rsp(void) {
    uintptr_t val;
    __asm__ volatile ("mov %%rsp, %0" : "=r"(val));
    return val;
}

/* Far return — used for switching to user mode (future scope) */
static inline void far_return(uint16_t cs, uint64_t rip, uint64_t rsp, uint16_t ss) {
    __asm__ volatile (
        "pushq %0\n"
        "pushq %1\n"
        "pushq %2\n"
        "pushq %3\n"
        "pushq %4\n"
        "iretq"
        :
        : "r"((uint64_t)ss), "r"(rsp), "r"(read_rflags()),
          "r"((uint64_t)cs), "r"(rip)
        : "memory"
    );
}

/* Swap GS base (used for per-CPU data in SMP) */
static inline void swapgs(void) {
    __asm__ volatile ("swapgs");
}

/* Load GDTR */
static inline void lgdt(void *gdtr) {
    __asm__ volatile ("lgdt (%0)" : : "r"(gdtr) : "memory");
}

/* Load IDTR */
static inline void lidt(void *idtr) {
    __asm__ volatile ("lidt (%0)" : : "r"(idtr) : "memory");
}

/* Load TR (task register) */
static inline void ltr(uint16_t sel) {
    __asm__ volatile ("ltr %0" : : "r"(sel));
}

/* Invalid TLB entry for address */
static inline void invlpg(uintptr_t addr) {
    __asm__ volatile ("invlpg (%0)" : : "r"(addr) : "memory");
}

/* CPUID instruction wrapper */
static inline void cpuid(uint32_t leaf, uint32_t subleaf,
                         uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
    __asm__ volatile ("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(subleaf));
}

/* Read MSR */
static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

/* Write MSR */
static inline void wrmsr(uint32_t msr, uint64_t val) {
    uint32_t low = (uint32_t)val;
    uint32_t high = (uint32_t)(val >> 32);
    __asm__ volatile ("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}

/* Cache flush helpers */
static inline void wbinvd(void) {
    __asm__ volatile ("wbinvd");
}

static inline void clflush(const void *addr) {
    __asm__ volatile ("clflush %0" : : "m"(*(const char *)addr));
}

/* Pause instruction (spinlock hint) */
static inline void pause(void) {
    __asm__ volatile ("pause");
}

/* Atomic compare-and-swap for 64-bit */
static inline bool atomic_cmpxchg64(volatile uint64_t *ptr, uint64_t expected, uint64_t desired) {
    uint64_t old;
    __asm__ volatile (
        "lock cmpxchgq %2, %1"
        : "=a"(old), "+m"(*ptr)
        : "r"(desired), "a"(expected)
        : "memory"
    );
    return old == expected;
}

/* Atomic fetch-and-add for 64-bit */
static inline uint64_t atomic_fetch_add64(volatile uint64_t *ptr, uint64_t val) {
    uint64_t old;
    __asm__ volatile (
        "lock xaddq %0, %1"
        : "=r"(old), "+m"(*ptr)
        : "0"(val)
        : "memory"
    );
    return old;
}

/* Compiler barrier */
#define barrier() __asm__ volatile ("" ::: "memory")

/* Macro to prevent unused variable warnings */
#define UNUSED(x) ((void)(x))

/* Pack structure tightly */
#define PACKED __attribute__((packed))

/* Align to boundary */
#define ALIGN(x) __attribute__((aligned(x)))

/* No return function attribute */
#define NORETURN __attribute__((noreturn))

/* Section placement */
#define SECTION(x) __attribute__((section(x)))

/* Used marker (prevents linker stripping) */
#define USED __attribute__((used))

#endif /* GOS_TYPES_H */

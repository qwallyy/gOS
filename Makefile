# gOS Makefile
# Reference: https://wiki.osdev.org/Makefile

# Cross-compiler toolchain (MANDATORY — never use host GCC)
# Reference: https://wiki.osdev.org/GCC_Cross-Compiler
CC      = x86_64-elf-gcc
CXX     = x86_64-elf-g++
LD      = x86_64-elf-ld
AS      = nasm
AR      = x86_64-elf-ar
OBJCOPY = x86_64-elf-objcopy

# Flags
# -mno-red-zone: disable red zone in ISR context
# -mno-mmx -mno-sse -mno-sse2: no FPU/SSE in kernel
# -ffreestanding: no standard library assumptions
# -nostdlib: do not link standard startup files or libraries
# -Wall -Wextra -Werror: strict warnings
# -O2: optimization level
# -g: debug symbols
CFLAGS  = -m64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
          -ffreestanding -nostdlib -nostdinc \
          -Wall -Wextra -Werror -Wno-unused-parameter -O2 -g \
          -I . -I include -I include/gOS \
          -std=c11 -pedantic \
          -fno-stack-protector -fno-pic -fno-pie -fno-builtin-popcount \
          -mcmodel=large

ASFLAGS = -f elf64 -g
LDFLAGS = -T linker/linker.ld -nostdlib -melf_x86_64

# Directories
BUILD_DIR = build
ISO_DIR   = iso

# Source files — auto-discovered, excluding tests (boot/test_*.asm), which each
# define their own `_start` and would collide at link time.
# boot/disk_boot.asm is a standalone [org 0x7c00] flat-binary boot sector; it is
# built separately (see the `disk_boot` target) and must NOT be linked into the
# ELF kernel, so it is excluded here too.
ASM_SOURCES := $(filter-out boot/disk_boot.asm,$(shell find boot arch hal -name "*.asm" ! -name "test_*.asm" | sort))
# Keep the multiboot2 header first so it lands in the first 32KB of the image.
# (There is no multiboot1 header: QEMU's multiboot loader rejects 64-bit ELFs
# outright, so QEMU -kernel loads us via the PVH note instead — see pvh_note.s.)
ASM_SOURCES := boot/multiboot2_header.asm $(filter-out boot/multiboot2_header.asm,$(ASM_SOURCES))
S_SOURCES   := boot/pvh_note.s
C_SOURCES   := $(filter-out boot/stub.c,$(shell find boot kernel hal drivers mm fs net lib proc sys power security ipc time acpi apic dma errno arch -name "*.c" | sort))

# Object files
ASM_OBJECTS := $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
S_OBJECTS   := $(patsubst %.s,$(BUILD_DIR)/%.o,$(S_SOURCES))
C_OBJECTS   := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
OBJECTS     := $(ASM_OBJECTS) $(S_OBJECTS) $(C_OBJECTS)

# Target
KERNEL_ELF  = $(BUILD_DIR)/kernel.elf
KERNEL_ISO  = gOS.iso
STUB_ELF    = $(BUILD_DIR)/stub.elf
STUB_BIN    = $(BUILD_DIR)/stub.bin
DISK_BOOT_BIN = $(BUILD_DIR)/disk_boot.bin

.PHONY: all clean iso run run-kernel run-stub debug disk_boot

all: $(KERNEL_ELF)

boot: $(STUB_BIN)

$(STUB_ELF): boot/stub.c boot/stub_linker.ld
	@mkdir -p $(dir $@)
	$(CC) -m32 -ffreestanding -nostdlib -fno-pie -fno-pic -O2 -c $< -o $(BUILD_DIR)/stub.o
	$(LD) -m elf_i386 -T boot/stub_linker.ld -nostdlib -o $@ $(BUILD_DIR)/stub.o
	@echo "[LINK] $@"

$(STUB_BIN): $(STUB_ELF) $(KERNEL_ELF)
	@mkdir -p $(dir $@)
	$(OBJCOPY) -O binary $< $@
	python3 boot/pad_stub.py $@ $(KERNEL_ELF)

$(KERNEL_ELF): $(OBJECTS) linker/linker.ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	@echo "[LINK] $@"

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@
	@echo "[ASM]  $@"

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[ASM]  $@"

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[CC]   $@"

# Standalone 512-byte disk boot sector (flat binary, not part of the ELF kernel)
disk_boot: $(DISK_BOOT_BIN)

$(DISK_BOOT_BIN): boot/disk_boot.asm
	@mkdir -p $(dir $@)
	$(AS) -f bin $< -o $@
	@echo "[BIN]  $@"

iso: $(KERNEL_ELF)
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	@mkrescue=""; \
	for c in grub-mkrescue grub2-mkrescue x86_64-elf-grub-mkrescue i686-elf-grub-mkrescue; do \
		if command -v $$c >/dev/null 2>&1; then mkrescue=$$c; break; fi; \
	done; \
	if [ -z "$$mkrescue" ]; then \
		echo "ERROR: grub-mkrescue not found (needs grub + xorriso + mtools). Install:"; \
		echo "  macOS:  brew install x86_64-elf-grub xorriso mtools"; \
		echo "  Debian: sudo apt-get install grub-pc-bin grub-common xorriso mtools"; \
		echo "Or skip the ISO and boot the kernel directly: make run-kernel"; \
		exit 1; \
	fi; \
	echo "Using $$mkrescue"; \
	$$mkrescue -o $(KERNEL_ISO) $(ISO_DIR)
	@echo "[ISO]  $(KERNEL_ISO)"

run: iso
	qemu-system-x86_64 \
		-cdrom $(KERNEL_ISO) \
		-m 256M \
		-serial stdio \
		-d int,cpu_reset \
		-no-reboot \
		-no-shutdown

# Boot the kernel ELF directly via QEMU's multiboot loader — no GRUB/ISO needed.
# Handy on hosts without grub-mkrescue (e.g. macOS).
run-kernel: $(KERNEL_ELF)
	qemu-system-x86_64 \
		-kernel $(KERNEL_ELF) \
		-m 256M \
		-serial stdio \
		-no-reboot \
		-no-shutdown

run-stub: $(STUB_BIN)
	qemu-system-x86_64 \
		-kernel $(STUB_BIN) \
		-m 256M \
		-serial stdio \
		-no-reboot \
		-no-shutdown

debug: iso
	qemu-system-x86_64 \
		-cdrom $(KERNEL_ISO) \
		-m 256M \
		-serial stdio \
		-s -S -no-reboot &
	@echo "QEMU GDB stub listening on :1234"
	@echo "Run: gdb -ex 'target remote :1234' -ex 'set arch i386:x86-64' $(KERNEL_ELF)"

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(KERNEL_ISO)

# Dependency generation
$(BUILD_DIR)/%.d: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM -MT $(BUILD_DIR)/$*.o $< > $@

-include $(patsubst %.c,$(BUILD_DIR)/%.d,$(C_SOURCES))

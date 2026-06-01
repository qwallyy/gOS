# Building gOS

## Prerequisites

You MUST use a cross-compiler. Never build the kernel with your host GCC.

```bash
# macOS (via Homebrew or build from source)
brew install x86_64-elf-gcc x86_64-elf-binutils nasm qemu

# Linux
sudo apt-get install gcc-x86-64-linux-gnu nasm qemu-system-x86 grub-common xorriso
```

## Build Steps

1. **Compile the kernel:**
   ```bash
   make
   ```

2. **Create bootable ISO:**
   ```bash
   make iso
   ```

3. **Run in QEMU:**
   ```bash
   make run
   ```

4. **Debug with GDB:**
   ```bash
   make debug
   # In another terminal:
   gdb -ex "target remote :1234" -ex "set arch i386:x86-64" build/kernel.elf
   ```

## Build Flags

- `-mno-red-zone`: Disable red zone (required for ISRs)
- `-mno-mmx -mno-sse -mno-sse2`: No FPU/SSE in kernel
- `-ffreestanding -nostdlib`: No standard library assumptions
- `-mcmodel=large`: Large memory model for higher half kernel

#!/bin/bash
# gOS Cross-Compiler Build Script
# Reference: https://wiki.osdev.org/GCC_Cross-Compiler

set -e

TARGET=x86_64-elf
PREFIX="$HOME/opt/cross"
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

BINUTILS_VERSION=2.41
GCC_VERSION=13.2.0

BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz"
GCC_URL="https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"

echo "================================"
echo "  gOS Cross-Compiler Builder"
echo "================================"
echo "Target:     $TARGET"
echo "Prefix:     $PREFIX"
echo "Jobs:       $PARALLEL_JOBS"
echo "Binutils:   $BINUTILS_VERSION"
echo "GCC:        $GCC_VERSION"
echo ""

mkdir -p "$PREFIX"
mkdir -p src build

export PATH="$PREFIX/bin:$PATH"

download_sources() {
    echo "Downloading sources..."
    if [ ! -f "src/binutils-${BINUTILS_VERSION}.tar.xz" ]; then
        curl -L -o "src/binutils-${BINUTILS_VERSION}.tar.xz" "$BINUTILS_URL"
    fi
    if [ ! -f "src/gcc-${GCC_VERSION}.tar.xz" ]; then
        curl -L -o "src/gcc-${GCC_VERSION}.tar.xz" "$GCC_URL"
    fi
}

extract_sources() {
    echo "Extracting sources..."
    if [ ! -d "src/binutils-${BINUTILS_VERSION}" ]; then
        tar -xf "src/binutils-${BINUTILS_VERSION}.tar.xz" -C src
    fi
    if [ ! -d "src/gcc-${GCC_VERSION}" ]; then
        tar -xf "src/gcc-${GCC_VERSION}.tar.xz" -C src
    fi
}

build_binutils() {
    echo "Building binutils..."
    mkdir -p build/binutils
    cd build/binutils
    ../../src/binutils-${BINUTILS_VERSION}/configure \
        --target=$TARGET \
        --prefix="$PREFIX" \
        --with-sysroot \
        --disable-nls \
        --disable-werror
    make -j$PARALLEL_JOBS
    make install
    cd ../..
}

build_gcc() {
    echo "Building GCC..."
    mkdir -p build/gcc
    cd build/gcc
    ../../src/gcc-${GCC_VERSION}/configure \
        --target=$TARGET \
        --prefix="$PREFIX" \
        --disable-nls \
        --enable-languages=c,c++ \
        --without-headers
    make all-gcc -j$PARALLEL_JOBS
    make all-target-libgcc -j$PARALLEL_JOBS
    make install-gcc
    make install-target-libgcc
    cd ../..
}

main() {
    download_sources
    extract_sources
    build_binutils
    build_gcc
    echo ""
    echo "Cross-compiler installed to $PREFIX"
    echo "Add to your PATH: export PATH=\"$PREFIX/bin:\$PATH\""
}

main

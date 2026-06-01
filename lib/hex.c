/* gOS Kernel — Hexadecimal Encoding/Decoding Implementation */

#include "lib/hex.h"
#include "lib/ctype.h"

static inline char hex_digit(uint8_t nibble) {
    return (nibble < 10) ? ('0' + nibble) : ('a' + nibble - 10);
}

static inline int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int hex_encode(const void *data, size_t len, char *out, size_t out_size) {
    if (!data || !out || out_size < len * 2 + 1) return -1;
    const uint8_t *bytes = data;
    size_t pos = 0;
    for (size_t i = 0; i < len; i++) {
        out[pos++] = hex_digit(bytes[i] >> 4);
        out[pos++] = hex_digit(bytes[i] & 0x0F);
    }
    out[pos] = '\0';
    return (int)pos;
}

int hex_decode(const char *hex, size_t hex_len, void *out, size_t out_size) {
    if (!hex || !out || hex_len % 2 != 0 || out_size < hex_len / 2) return -1;
    uint8_t *bytes = out;
    size_t pos = 0;
    for (size_t i = 0; i < hex_len; i += 2) {
        int hi = hex_value(hex[i]);
        int lo = hex_value(hex[i + 1]);
        if (hi < 0 || lo < 0) return -1;
        bytes[pos++] = (uint8_t)((hi << 4) | lo);
    }
    return (int)pos;
}

bool hex_is_valid(const char *hex, size_t len) {
    if (!hex || len % 2 != 0) return false;
    for (size_t i = 0; i < len; i++) {
        if (hex_value(hex[i]) < 0) return false;
    }
    return true;
}

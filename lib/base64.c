/* gOS Kernel — Base64 Encoding/Decoding Implementation */

#include "lib/base64.h"

static const char g_base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t base64_encoded_size(size_t raw_len) {
    return ((raw_len + 2) / 3) * 4;
}

size_t base64_decoded_size(const char *b64, size_t b64_len) {
    size_t padding = 0;
    if (b64_len > 0 && b64[b64_len - 1] == '=') padding++;
    if (b64_len > 1 && b64[b64_len - 2] == '=') padding++;
    return (b64_len / 4) * 3 - padding;
}

int base64_encode(const void *data, size_t len, char *out, size_t out_size) {
    if (!data || !out) return -1;
    size_t need = base64_encoded_size(len) + 1;
    if (out_size < need) return -1;

    const uint8_t *in = data;
    size_t pos = 0;
    for (size_t i = 0; i < len; i += 3) {
        uint32_t val = ((uint32_t)in[i]) << 16;
        if (i + 1 < len) val |= ((uint32_t)in[i + 1]) << 8;
        if (i + 2 < len) val |= in[i + 2];

        out[pos++] = g_base64_table[(val >> 18) & 0x3F];
        out[pos++] = g_base64_table[(val >> 12) & 0x3F];
        out[pos++] = (i + 1 < len) ? g_base64_table[(val >> 6) & 0x3F] : '=';
        out[pos++] = (i + 2 < len) ? g_base64_table[val & 0x3F] : '=';
    }
    out[pos] = '\0';
    return (int)pos;
}

static inline int base64_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

int base64_decode(const char *b64, size_t b64_len, void *out, size_t out_size) {
    if (!b64 || !out || b64_len % 4 != 0) return -1;
    size_t need = base64_decoded_size(b64, b64_len);
    if (out_size < need) return -1;

    uint8_t *dst = out;
    size_t pos = 0;
    for (size_t i = 0; i < b64_len; i += 4) {
        int a = base64_value(b64[i]);
        int b = base64_value(b64[i + 1]);
        int c = base64_value(b64[i + 2]);
        int d = base64_value(b64[i + 3]);
        if (a < 0 || b < 0) return -1;

        uint32_t val = ((uint32_t)a << 18) | ((uint32_t)b << 12);
        if (c >= 0) val |= (uint32_t)c << 6;
        if (d >= 0) val |= (uint32_t)d;

        dst[pos++] = (uint8_t)(val >> 16);
        if (c >= 0) dst[pos++] = (uint8_t)(val >> 8);
        if (d >= 0) dst[pos++] = (uint8_t)val;
    }
    return (int)pos;
}

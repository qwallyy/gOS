/* gOS Kernel — Hexadecimal Encoding/Decoding */

#ifndef GOS_HEX_H
#define GOS_HEX_H

#include "gOS/types.h"

int hex_encode(const void *data, size_t len, char *out, size_t out_size);
int hex_decode(const char *hex, size_t hex_len, void *out, size_t out_size);
bool hex_is_valid(const char *hex, size_t len);

#endif /* GOS_HEX_H */

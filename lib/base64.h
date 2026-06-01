/* gOS Kernel — Base64 Encoding/Decoding */

#ifndef GOS_BASE64_H
#define GOS_BASE64_H

#include "gOS/types.h"

int base64_encode(const void *data, size_t len, char *out, size_t out_size);
int base64_decode(const char *b64, size_t b64_len, void *out, size_t out_size);
size_t base64_encoded_size(size_t raw_len);
size_t base64_decoded_size(const char *b64, size_t b64_len);

#endif /* GOS_BASE64_H */

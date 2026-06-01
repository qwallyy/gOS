/* gOS Kernel — TFTP Client Stub
 * Reference: https://wiki.osdev.org/TFTP
 */

#ifndef GOS_TFTP_H
#define GOS_TFTP_H

#include "gOS/types.h"

#define TFTP_PORT           69
#define TFTP_OPCODE_READ    1
#define TFTP_OPCODE_WRITE   2
#define TFTP_OPCODE_DATA    3
#define TFTP_OPCODE_ACK     4
#define TFTP_OPCODE_ERROR   5

typedef struct tftp_packet {
    uint16_t opcode;
    uint8_t  payload[];
} PACKED tftp_packet_t;

void tftp_init(void);
bool tftp_read_file(const char *server_ip, const char *filename, void *buf, size_t max_size, size_t *out_size);
void tftp_receive(const void *packet, size_t len);

#endif /* GOS_TFTP_H */

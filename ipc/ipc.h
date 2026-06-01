/* gOS Kernel — Inter-Process Communication (IPC)
 * Reference: https://wiki.osdev.org/IPC
 */

#ifndef GOS_IPC_H
#define GOS_IPC_H

#include "gOS/types.h"

#define IPC_MSG_MAX_SIZE    4096
#define IPC_MSG_MAX_QUEUE   64

typedef struct ipc_message {
    uint32_t sender_pid;
    uint32_t type;
    size_t   size;
    uint8_t  data[IPC_MSG_MAX_SIZE];
} ipc_message_t;

void ipc_init(void);
int  ipc_send(uint32_t dest_pid, const ipc_message_t *msg);
int  ipc_receive(uint32_t src_pid, ipc_message_t *msg, uint64_t timeout_ms);
int  ipc_broadcast(const ipc_message_t *msg);
void ipc_flush_queue(uint32_t pid);

#endif /* GOS_IPC_H */

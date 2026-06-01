/* gOS Kernel — IPC Implementation
 * Reference: https://wiki.osdev.org/IPC
 */

#include "ipc/ipc.h"
#include "proc/proc.h"
#include "mm/heap/heap.h"
#include "gOS/klog.h"
#include "gOS/errno.h"
#include "lib/string.h"

typedef struct ipc_queue {
    ipc_message_t *messages;
    size_t head;
    size_t tail;
    size_t count;
    uint32_t owner_pid;
} ipc_queue_t;

static ipc_queue_t *g_ipc_queues = NULL;
static uint32_t g_ipc_max_queues = 0;

void ipc_init(void) {
    klog_info("IPC subsystem initializing");
    g_ipc_max_queues = PROC_MAX_PROCESSES;
    g_ipc_queues = kzalloc(sizeof(ipc_queue_t) * g_ipc_max_queues);
    if (!g_ipc_queues) {
        klog_emerg("IPC: failed to allocate message queues");
        return;
    }
    for (uint32_t i = 0; i < g_ipc_max_queues; i++) {
        g_ipc_queues[i].messages = NULL;
        g_ipc_queues[i].owner_pid = 0;
    }
    klog_info("IPC: %u message queues allocated", g_ipc_max_queues);
}

static ipc_queue_t *ipc_get_queue(uint32_t pid) {
    if (pid >= g_ipc_max_queues) return NULL;
    ipc_queue_t *q = &g_ipc_queues[pid];
    if (q->owner_pid == 0) {
        q->messages = kzalloc(sizeof(ipc_message_t) * IPC_MSG_MAX_QUEUE);
        if (!q->messages) return NULL;
        q->owner_pid = pid;
        q->head = 0;
        q->tail = 0;
        q->count = 0;
    }
    return q;
}

int ipc_send(uint32_t dest_pid, const ipc_message_t *msg) {
    if (!msg || msg->size > IPC_MSG_MAX_SIZE) return -EINVAL;
    process_t *dest = proc_get_by_pid(dest_pid);
    if (!dest) return -ESRCH;

    ipc_queue_t *q = ipc_get_queue(dest_pid);
    if (!q || !q->messages) return -ENOMEM;
    if (q->count >= IPC_MSG_MAX_QUEUE) return -EAGAIN;

    ipc_message_t *slot = &q->messages[q->head];
    memcpy(slot, msg, sizeof(ipc_message_t));
    slot->sender_pid = proc_get_current() ? proc_get_current()->pid : 0;

    q->head = (q->head + 1) % IPC_MSG_MAX_QUEUE;
    q->count++;

    klog_debug("IPC: message sent from PID %u to PID %u (type=%u)",
               slot->sender_pid, dest_pid, msg->type);
    return 0;
}

int ipc_receive(uint32_t src_pid, ipc_message_t *msg, uint64_t timeout_ms) {
    (void)timeout_ms;
    if (!msg) return -EINVAL;
    process_t *current = proc_get_current();
    if (!current) return -ESRCH;

    ipc_queue_t *q = ipc_get_queue(current->pid);
    if (!q || !q->messages) return -ENOMEM;

    /* Wait for a message */
    while (q->count == 0) {
        proc_yield();
        if (q->count == 0) {
            /* Timeout or no message - for stub, return EAGAIN */
            return -EAGAIN;
        }
    }

    /* Find message matching src_pid filter */
    for (size_t i = 0; i < IPC_MSG_MAX_QUEUE; i++) {
        size_t idx = (q->tail + i) % IPC_MSG_MAX_QUEUE;
        ipc_message_t *candidate = &q->messages[idx];
        if (candidate->sender_pid == 0) continue;
        if (src_pid == 0 || candidate->sender_pid == src_pid) {
            memcpy(msg, candidate, sizeof(ipc_message_t));
            candidate->sender_pid = 0;
            q->tail = (idx + 1) % IPC_MSG_MAX_QUEUE;
            q->count--;
            return 0;
        }
    }
    return -EAGAIN;
}

int ipc_broadcast(const ipc_message_t *msg) {
    if (!msg) return -EINVAL;
    int sent = 0;
    for (uint32_t i = 1; i < g_ipc_max_queues; i++) {
        process_t *p = proc_get_by_pid(i);
        if (p && p->state != PROC_STATE_UNUSED) {
            if (ipc_send(i, msg) == 0) sent++;
        }
    }
    klog_debug("IPC: broadcast to %d processes", sent);
    return sent;
}

void ipc_flush_queue(uint32_t pid) {
    ipc_queue_t *q = ipc_get_queue(pid);
    if (!q) return;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    if (q->messages) {
        for (size_t i = 0; i < IPC_MSG_MAX_QUEUE; i++) {
            q->messages[i].sender_pid = 0;
        }
    }
}

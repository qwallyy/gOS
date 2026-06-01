/* gOS Kernel — Semaphore Implementation
 * Reference: https://wiki.osdev.org/Semaphores
 */

#include "ipc/semaphore.h"
#include "proc/proc.h"
#include "gOS/klog.h"

void semaphore_init(semaphore_t *sem, int32_t initial) {
    if (!sem) return;
    sem->count = initial;
    sem->waiters = 0;
}

void semaphore_wait(semaphore_t *sem) {
    if (!sem) return;
    sem->waiters++;
    while (sem->count <= 0) {
        proc_yield();
    }
    sem->count--;
    sem->waiters--;
}

bool semaphore_try_wait(semaphore_t *sem) {
    if (!sem) return false;
    if (sem->count > 0) {
        sem->count--;
        return true;
    }
    return false;
}

void semaphore_signal(semaphore_t *sem) {
    if (!sem) return;
    sem->count++;
}

int32_t semaphore_get_value(const semaphore_t *sem) {
    if (!sem) return -1;
    return sem->count;
}

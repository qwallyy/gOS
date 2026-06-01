/* gOS Kernel — Semaphores
 * Reference: https://wiki.osdev.org/Semaphores
 */

#ifndef GOS_SEMAPHORE_H
#define GOS_SEMAPHORE_H

#include "gOS/types.h"

typedef struct semaphore {
    volatile int32_t count;
    volatile int32_t waiters;
} semaphore_t;

void semaphore_init(semaphore_t *sem, int32_t initial);
void semaphore_wait(semaphore_t *sem);
bool semaphore_try_wait(semaphore_t *sem);
void semaphore_signal(semaphore_t *sem);
int32_t semaphore_get_value(const semaphore_t *sem);

#endif /* GOS_SEMAPHORE_H */

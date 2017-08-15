#include "lock.h"
#include <intr.h>

void init_lock(struct lock_t *lock) {
    lock->spin = 0;
    INIT_LIST_HEAD(&(lock->wait));
}

unsigned int lockup(struct lock_t *lock) {
    unsigned int old_ie;

    old_ie = disable_interrupts();
    if (lock->spin) {
    }
    lock->spin = 1;
    if (old_ie) {
        enable_interrupts();
    }

    return 1;
}

unsigned int unlock(struct lock_t *lock) {
    unsigned int old_ie;

    old_ie = disable_interrupts();
    if (lock->spin) {
        lock->spin = 0;
    }
    if (old_ie) {
        enable_interrupts();
    }

    return 1;
}

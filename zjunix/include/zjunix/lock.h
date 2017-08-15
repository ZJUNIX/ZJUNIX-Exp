#ifndef _ZJUNIX_LOCK_H
#define _ZJUNIX_LOCK_H

#include <zjunix/list.h>

struct lock_t {
    unsigned int spin;
    struct list_head wait;
};

extern void init_lock(struct lock_t *lock);
extern unsigned int lockup(struct lock_t *lock);
extern unsigned int unlock(struct lock_t *lock);

#endif  // !_ZJUNIX_LOCK_H
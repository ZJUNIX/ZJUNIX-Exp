#include "intr.h"
#include "arch.h"

#pragma GCC push_options
#pragma GCC optimize("O0")

intr_fn interrupts[8];

void init_interrupts() {
    // status 0000 0000 0000 0000 1001 1100 0000 0001
    // cause 0000 0000 1000 0000 0000 0000 0000 0000
    unsigned int t;
    asm volatile(
        "mfc0 $t0, $12\n\t"
        "ori $t0, $t0, 0x1\n\t"
        "move %0, $t0\n\t"
        "mtc0 $t0, $12"
        : "=r"(t));
}

int enable_interrupts() {
    int old = 0;
    asm volatile(
        "mfc0 $t0, $12\n\t"
        "andi %0, $t0, 0x1\n\t"
        "ori $t0, $t0, 0x1\n\t"
        "mtc0 $t0, $12"
        : "=r"(old));
    return old;
}

int disable_interrupts() {
    int old = 0;
    asm volatile(
        "mfc0 $t0, $12\n\t"
        "andi %0, $t0, 0x1\n\t"
        "li $t1, 0xfffffffe\n\t"
        "and $t0, $t0, $t1\n\t"
        "mtc0 $t0, $12"
        : "=r"(old));
    return old;
}

void do_interrupts(unsigned int status, unsigned int cause, context* pt_context) {
    int i;
    int index = cause >> 8;
    for (i = 0; i < 8; i++) {
        if ((index & 1) && interrupts[i] != 0) {
            interrupts[i](status, cause, pt_context);
        }
        index >>= 1;
    }
}

void register_interrupt_handler(int index, intr_fn fn) {
    index &= 7;
    interrupts[index] = fn;
    index = 1 << (index + 8);
    asm volatile(
        "mfc0 $t0, $12\n\t"
        "or $t0, $t0, %0\n\t"
        "mtc0 $t0, $12"
        : "=r"(index));
}

#pragma GCC pop_options
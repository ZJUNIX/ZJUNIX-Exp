#include <exc.h>
#include <zjunix/syscall.h>
#include "syscall4.h"

sys_fn syscalls[256];

void init_syscall() {
    register_exception_handler(8, syscall);

    // register all syscalls here
    register_syscall(4, syscall4);
}

void syscall(unsigned int status, unsigned int cause, unsigned int* sp) {
    unsigned int a0, a1, a2, a3, v0;
    a0 = *(sp + 4);
    a1 = *(sp + 5);
    a2 = *(sp + 6);
    a3 = *(sp + 7);
    v0 = *(sp + 2) & 255;
    *(sp + 0) += 4;  // EPC
    if (syscalls[v0]) {
        syscalls[v0](a0, a1, a2, a3);
    }
}

void register_syscall(int index, sys_fn fn) {
    index &= 255;
    syscalls[index] = fn;
}

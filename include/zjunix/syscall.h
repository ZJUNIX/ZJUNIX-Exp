#ifndef _ZJUNIX_SYSCALL_H
#define _ZJUNIX_SYSCALL_H

#include <zjunix/pc.h>

typedef void (*sys_fn)(unsigned int status, unsigned int cause, context* pt_context);

extern sys_fn syscalls[256];

void init_syscall();
void syscall(unsigned int status, unsigned int cause, context* pt_context);
void register_syscall(int index, sys_fn fn);

#endif // ! _ZJUNIX_SYSCALL_H

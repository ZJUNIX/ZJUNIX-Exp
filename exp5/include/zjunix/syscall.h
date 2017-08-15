#ifndef _ZJUNIX_SYSCALL_H
#define _ZJUNIX_SYSCALL_H

typedef void(*sys_fn)(unsigned int, unsigned int, unsigned int, unsigned int);

extern sys_fn syscalls[256];

void init_syscall();
void syscall(unsigned int status, unsigned int cause, unsigned int* sp);
void register_syscall(int index, sys_fn fn);

#endif // ! _ZJUNIX_SYSCALL_H

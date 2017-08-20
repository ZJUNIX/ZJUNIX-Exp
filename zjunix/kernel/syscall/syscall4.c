
#include <arch.h>
#include <zjunix/syscall.h>

void syscall4(unsigned int status, unsigned int cause, context* pt_context) {
    kernel_puts((unsigned char*)pt_context->a0,0xfff,0);
}

#include <arch.h>
#include <zjunix/syscall.h>

void syscall4(unsigned int status, unsigned int cause, context* pt_context) {
    *GPIO_LED = pt_context->a0;
}
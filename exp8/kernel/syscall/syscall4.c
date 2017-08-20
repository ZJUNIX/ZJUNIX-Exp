
#include <arch.h>
#include <zjunix/syscall.h>
#include <driver/vga.h>

void syscall4(unsigned int status, unsigned int cause, context* pt_context) {
    kernel_puts((unsigned char*)pt_context->a0,0xfff,0);
}
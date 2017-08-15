
#include <arch.h>
#include <zjunix/syscall.h>

void syscall4(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3) {
    *GPIO_LED = a0;
}
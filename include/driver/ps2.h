#ifndef _DRIVER_PS2
#define _DRIVER_PS2

#include <intr.h>
#include <zjunix/pc.h>

void init_ps2();
void ps2_handler(unsigned int status, unsigned int cause, context* pt_context);
int kernel_getkey();
int kernel_getchar();

#endif // ! _DRIVER_PS2
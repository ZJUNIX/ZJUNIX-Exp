#ifndef _ASSERT_H
#define _ASSERT_H

#include <driver/vga.h>

#undef assert
void assert(int statement, char * message);

#endif // ! _ASSERT_H
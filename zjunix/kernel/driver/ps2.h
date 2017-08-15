#ifndef _PS2_H
#define _PS2_H

#include <driver/ps2.h>

int kernel_scantoascii(int key);

#ifdef PS2_DEBUG
void print_wptr();
void print_rptr();
void print_buffer();
void print_curr_key(int key);
void print_curr_char(int key);
#endif  // ! PS2_DEBUG

#endif

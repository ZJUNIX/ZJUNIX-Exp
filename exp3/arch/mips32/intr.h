#ifndef _INTR_H
#define _INTR_H

typedef void (*intr_fn)(unsigned int, unsigned int, unsigned int*);

extern intr_fn interrupts[8];

void init_interrupts();
int enable_interrupts();
int disable_interrupts();
void do_interrupts(unsigned int status, unsigned int cause, unsigned int* sp);
void register_interrupt_handler(int index, intr_fn fn);

#endif

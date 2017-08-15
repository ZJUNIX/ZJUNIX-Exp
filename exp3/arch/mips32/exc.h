#ifndef _EXC_H
#define _EXC_H

typedef void (*exc_fn)(unsigned int, unsigned int, unsigned int*);

extern exc_fn exceptions[32];

void do_exceptions(unsigned int status, unsigned int cause, unsigned int* sp);
void register_exception_handler(int index, exc_fn fn);
void init_exception();

#endif

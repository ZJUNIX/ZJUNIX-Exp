#ifndef _EXC_H
#define _EXC_H

#include <zjunix/pc.h>

typedef void (*exc_fn)(unsigned int, unsigned int, context*);

extern exc_fn exceptions[32];

void do_exceptions(unsigned int status, unsigned int cause, context* pt_context);
void register_exception_handler(int index, exc_fn fn);
void init_exception();

#endif
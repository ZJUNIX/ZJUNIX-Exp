#ifndef _TIME_H
#define _TIME_H
#include <intr.h>
#include <zjunix/time.h>

void get_time_string(unsigned int ticks_high, unsigned int ticks_low, char *buf);

extern unsigned int month;
extern unsigned int day;
extern unsigned int year;
extern unsigned int hour;
extern unsigned int minute;
extern unsigned int second;

#endif

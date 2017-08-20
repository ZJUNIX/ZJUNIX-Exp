#include "time.h"
#include <assert.h>
#include <driver/vga.h>
#include <intr.h>
#include <zjunix/pc.h>

void get_time_string(unsigned int ticks_high, unsigned int ticks_low, char *buf) {
    // Divide by 256
    ticks_low = (ticks_low >> 8) | (ticks_high << 24);
    ticks_high >>= 8;
    // 2^32 / 390625: q = 10995, r = 45421, r^2 less than 32 bits.
    //(A*2^32+B)/f = (A%f)*2^32/f + B/f = (A%f)*q+((A%f)*r)%f + B/f
    unsigned int second;
    second = (ticks_high % 390625);
    second = second * 10995 + (second * 45421) % 390625;
    second += ticks_low / 390625;

    unsigned int minute = second / 60;
    unsigned int hour = minute / 60;
    second %= 60;
    minute %= 60;
    hour %= 24;
    buf[0] = hour / 10 + '0';
    buf[1] = hour % 10 + '0';
    buf[2] = ':';
    buf[3] = minute / 10 + '0';
    buf[4] = minute % 10 + '0';
    buf[5] = ':';
    buf[6] = second / 10 + '0';
    buf[7] = second % 10 + '0';
}

#pragma GCC push_options
#pragma GCC optimize("O0")

void system_time_proc() {
    unsigned int ticks_high, ticks_low;
    int i;
    char buffer[8];
    char *day = "01/07/2016 ";
    while (1) {
        asm volatile(
            "mfc0 %0, $9, 6\n\t"
            "mfc0 %1, $9, 7\n\t"
            : "=r"(ticks_low), "=r"(ticks_high));
        get_time_string(ticks_high, ticks_low, buffer);

        for (i = 0; i < 11; i++)
            kernel_putchar_at(day[i], 0xfff, 0, 29, 61 + i);
        for (i = 0; i < 8; i++)
            kernel_putchar_at(buffer[i], 0xfff, 0, 29, 72 + i);
    }
}

void get_time(char *buf, int len) {
    assert(len >= 9, "Buf of get_time too small, at least 9 bytes");
    unsigned int ticks_high, ticks_low;
    asm volatile(
        "mfc0 %0, $9, 6\n\t"
        "mfc0 %1, $9, 7\n\t"
        : "=r"(ticks_low), "=r"(ticks_high));
    get_time_string(ticks_high, ticks_low, buf);
    buf[8] = 0;
}

#pragma GCC pop_options
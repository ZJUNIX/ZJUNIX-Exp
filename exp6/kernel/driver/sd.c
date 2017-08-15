#include "sd.h"
#include <driver/vga.h>

#pragma GCC push_opitons
#pragma GCC optimize("O0")

static volatile unsigned int* const SD_CTRL = (unsigned int*)0xbfc09100;
static volatile unsigned int* const SD_BUF = (unsigned int*)0xbfc08000;

static int sd_send_cmd_blocking(int cmd, int argument) {
    int t;
    SD_CTRL[1] = cmd;
    SD_CTRL[0] = argument;
    t = 4096;
    while (t--)
        ;  // Wait for command transaction
    do {
        t = SD_CTRL[13];
    } while (t == 0);
    if (t & 1)
        return 0;
    else
        return t;
}

int sd_read_sector_blocking(int id, void* buffer) {
    // Disable interrupts
    unsigned int prev_status;
    asm volatile(
        "mfc0 $t0, $12\n\t"
        "move %0, $t0\n\t"
        "li $t1, -2\n\t"
        "and $t0, $t0, $t1\n\t"
        "mtc0 $t0, $12\n\t"
        : "=r"(prev_status));
    int code;
    int* buffer_int = (int*)buffer;
    int i;

    SD_CTRL[18] = 0;  // DMA address
    SD_CTRL[15] = 0;  // Data transfer events
    code = sd_send_cmd_blocking(0x1139, id);
    if (code != 0)
        goto ret;
    do {
        code = SD_CTRL[15];
    } while (code == 0);
    if (code & 1) {
        for (i = 0; i < 128; i++)
            buffer_int[i] = SD_BUF[i];
        code = 0;
    }
ret:
    // Enable interrupts
    asm volatile("mtc0 %0, $12\n\t" : : "r"(prev_status));
    return code;
}

int sd_write_sector_blocking(int id, void* buffer) {
    // Disable interrupts
    unsigned int prev_status;
    asm volatile(
        "mfc0 $t0, $12\n\t"
        "move %0, $t0\n\t"
        "li $t1, -2\n\t"
        "and $t0, $t0, $t1\n\t"
        "mtc0 $t0, $12\n\t"
        : "=r"(prev_status));
    int code;
    int* buffer_int = (int*)buffer;
    int i;
    SD_CTRL[18] = 0;  // DMA address
    SD_CTRL[15] = 0;  // Clear data transfer events
    // Fill buffer
    for (i = 0; i < 128; i++)
        SD_BUF[i] = buffer_int[i];
    code = sd_send_cmd_blocking(0x1859, id);
    if (code != 0)
        goto ret;
    do {
        code = SD_CTRL[15];
    } while (code == 0);
    if (code & 1)
        code = 0;
ret:
    // Enable interrupts
    asm volatile("mtc0 %0, $12\n\t" : : "r"(prev_status));
    return code;
}

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

u32 sd_read_block(unsigned char* buf, unsigned long addr, unsigned long count) {
    u32 i;
    u32 result;
    if (1 == count) {
        // read single block
        return sd_read_sector_blocking(addr, buf);
    } else {
        // read multiple block
        for (i = 0; i < count; ++i) {
            result = sd_read_sector_blocking(addr + i, buf + i * SECSIZE);
            if (0 != result) {
                return 1;
            }
        }
        return 0;
    }
}

u32 sd_write_block(unsigned char* buf, unsigned long addr, unsigned long count) {
    u32 i;
    u32 result;
#ifdef SD_DEBUG
    kernel_printf("Count: %x, Addr: %x", count, addr);
#endif
    if (1 == count) {
        // write single block
        return sd_write_sector_blocking(addr, buf);
    } else {
        // write multiple block
        for (i = 0; i < count; ++i) {
            result = sd_write_sector_blocking(addr + i, buf + i * SECSIZE);
            if (0 != result) {
                kernel_printf("Error: sd_write_sector_blocking failed:%x\n", result);
                kernel_printf("index=%x, buf=%x\n", addr + i, (int)(buf + i * SECSIZE));
                return 1;
            }
        }
        return 0;
    }
}

#pragma GCC pop_options
#include <driver/vga.h>
#include <zjunix/utils.h>

void* kernel_memcpy(void* dest, void* src, int len) {
    char* deststr = dest;
    char* srcstr = src;
    while (len--) {
        *deststr = *srcstr;
        deststr++;
        srcstr++;
    }
    return dest;
}

#pragma GCC push_options
#pragma GCC optimize("O2")
void* kernel_memset(void* dest, int b, int len) {
#ifdef MEMSET_DEBUG
    kernel_printf("memset:%x,%x,len%x,", (int)dest, b, len);
#endif  // ! MEMSET_DEBUG
    char content = b ? -1 : 0;
    char* deststr = dest;
    while (len--) {
        *deststr = content;
        deststr++;
    }
#ifdef MEMSET_DEBUG
    kernel_printf("%x\n", (int)deststr);
#endif  // ! MEMSET_DEBUG
    return dest;
}
#pragma GCC pop_options

unsigned int* kernel_memset_word(unsigned int* dest, unsigned int w, int len) {
    while (len--)
        *dest++ = w;

    return dest;
}

int kernel_strcmp(const char* dest, const char* src) {
    while ((*dest == *src) && (*dest != 0)) {
        dest++;
        src++;
    }
    return *dest - *src;
}

char* kernel_strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++))
        ;
    return dest;
}

int pow(int x, int z) {
    int ret = 1;
    if (z < 0)
        return -1;
    while (z--) {
        ret *= x;
    }
    return ret;
}

#pragma GCC push_options
#pragma GCC optimize("O0")

void kernel_cache(unsigned int block_index) {
    block_index = block_index | 0x80000000;
    asm volatile(
        "li $t0, 233\n\t"
        "mtc0 $t0, $8\n\t"
        "move $t0, %0\n\t"
        "cache 0, 0($t0)\n\t"
        "nop\n\t"
        "cache 1, 0($t0)\n\t"
        : "=r"(block_index));
}

#pragma GCC pop_options

void kernel_serial_puts(char* str) {
    while (*str)
        *((unsigned int*)0xbfc09018) = *str++;
}

void kernel_serial_putc(char c) {
    *((unsigned int*)0xbfc09018) = c;
}

unsigned int is_bound(unsigned int val, unsigned int bound) {
    return !(val & (bound - 1));
}

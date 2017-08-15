#ifndef _ZJUNIX_UITILS_H
#define _ZJUNIX_UITILS_H

#define container_of(ptr, type, member) ((type*)((char*)ptr - (char*)&(((type*)0)->member)))

void* kernel_memcpy(void* dest, void* src, int len);
void* kernel_memset(void* dest, int b, int len);
unsigned int* kernel_memset_word(unsigned int* dest, unsigned int w, int len);
int kernel_strcmp(const char* dest, const char* src);
int pow(int x, int z);
void kernel_cache(unsigned int block_index);
char* kernel_strcpy(char* dest, const char* src);
void kernel_serial_puts(char* str);
void kernel_serial_putc(char c);
unsigned int is_bound(unsigned int val, unsigned int bound);

typedef unsigned char* va_list;
#define _INTSIZEOF(n) ((sizeof(n) + sizeof(unsigned int) - 1) & ~(sizeof(unsigned int) - 1))
#define va_start(ap, v) (ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap, t) (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap) (ap = (va_list)0)

#endif // ! _ZJUNIX_UITILS_H

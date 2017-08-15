#include "exec.h"

#include <driver/ps2.h>
#include <driver/vga.h>
#include <zjunix/fs/fat.h>
#include <zjunix/pc.h>
#include <zjunix/slab.h>
#include <zjunix/utils.h>

#pragma GCC push_options
#pragma GCC optimize("O0")
FILE file;
const unsigned int CACHE_BLOCK_SIZE = 64;

int exec(char* filename) {
    unsigned char buffer[512];
    int result = fs_open(&file, filename);
    if (result != 0) {
        kernel_printf("File %s not exist\n", filename);
        return 1;
    }
    unsigned int size = get_entry_filesize(file.entry.data);
    unsigned int n = size / CACHE_BLOCK_SIZE + 1;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int ENTRY = (unsigned int)kmalloc(4096);
    for (j = 0; j < n; j++) {
        fs_read(&file, buffer, CACHE_BLOCK_SIZE);
        kernel_memcpy((void*)(ENTRY + j * CACHE_BLOCK_SIZE), buffer, CACHE_BLOCK_SIZE);
        kernel_cache(ENTRY + j * CACHE_BLOCK_SIZE);
    }
    unsigned int cp0EntryLo0 = ((ENTRY >> 6) & 0x01ffffc0) | 0x1e;
    asm volatile(
        "li $t0, 1\n\t"
        "mtc0 $t0, $10\n\t"
        "mtc0 $zero, $5\n\t"
        "move $t0, %0\n\t"
        "mtc0 $t0, $2\n\t"
        "mtc0 $zero, $3\n\t"
        "mtc0 $zero, $0\n\t"
        "nop\n\t"
        "nop\n\t"
        "tlbwi"
        : "=r"(cp0EntryLo0));
    int (*f)() = (int (*)())(0);
#ifdef EXEC_DEBUG
    kernel_printf("Exec load at: 0x%x\n", ENTRY);
#endif  // ! EXEC_DEBUG
    int r = f();
    kfree((void*)ENTRY);
    return r;
}
#pragma GCC pop_options
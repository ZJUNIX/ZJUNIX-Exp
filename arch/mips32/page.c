#include "page.h"
#include <zjunix/utils.h>
#include "arch.h"

#pragma GCC push_options
#pragma GCC optimize("O0")

void init_pgtable() {
    asm volatile(
        "mtc0 $zero, $2\n\t"
        "mtc0 $zero, $3\n\t"
        "mtc0 $zero, $5\n\t"
        "mtc0 $zero, $10\n\t"

        "move $v0, $zero\n\t"
        "li $v1, 32\n"

        "init_pgtable_L1:\n\t"
        "mtc0 $v0, $0\n\t"
        "addi $v0, $v0, 1\n\t"
        "bne $v0, $v1, init_pgtable_L1\n\t"
        "tlbwi\n\t"
        "nop");
}

#pragma GCC pop_options
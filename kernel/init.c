#include <arch.h>
#include <exc.h>
#include <init_place_holder.h>
#include <intr.h>
#include <zjunix/syscall.h>

void test_syscall4() {
    asm volatile(
        "li $a0, 0x00ff\n\t"
        "li $v0, 4\n\t"
        "syscall\n\t"
        "nop\n\t");
}

void init_kernel() {
    // Exception
    init_exception();
    // System call
    init_syscall();
    // Page table
    init_pgtable();
    // Drivers
    init_vga();
    init_ps2();
    // Memory management
    init_mem();
    // File system
    init_fs();
    // Process control
    init_pc();
    // Interrupts
    init_interrupts();
    // Init finished, write seg
    *GPIO_SEG = 0x11223344;
    test_syscall4();
    // Halt
    while (1)
        ;
}

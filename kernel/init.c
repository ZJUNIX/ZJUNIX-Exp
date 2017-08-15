#include <arch.h>
#include <init_place_holder.h>

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
    // Halt
    while(1);
}

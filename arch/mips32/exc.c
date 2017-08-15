#include "exc.h"

#include <driver/vga.h>
#include <zjunix/pc.h>

#pragma GCC push_options
#pragma GCC optimize("O0")

exc_fn exceptions[32];

void do_exceptions(unsigned int status, unsigned int cause, context* pt_context) {
    int index = cause >> 2;
    index &= 0x1f;
    if (exceptions[index]) {
        exceptions[index](status, cause, pt_context);
    } else {
        task_struct* pcb;
        unsigned int badVaddr;
        asm volatile("mfc0 %0, $8\n\t" : "=r"(badVaddr));
        pcb = get_curr_pcb();
        kernel_printf("\nProcess %s exited due to exception cause=%x;\n", pcb->name, cause);
        kernel_printf("status=%x, EPC=%x, BadVaddr=%x\n", status, pcb->context.epc, badVaddr);
        pc_kill_syscall(status, cause, pt_context);
        while (1)
            ;
    }
}

void register_exception_handler(int index, exc_fn fn) {
    index &= 31;
    exceptions[index] = fn;
}

void init_exception() {
    // status 0000 0000 0000 0000 0000 0000 0000 0000
    // cause 0000 0000 1000 0000 0000 0000 0000 0000
    asm volatile(
        "mtc0 $zero, $12\n\t"
        "li $t0, 0x800000\n\t"
        "mtc0 $t0, $13\n\t");
}

#pragma GCC pop_options
#include "interrupt.h"
#include "com.h"
#include <asm/asm.h>
#include <libmacros.h>

void interrupt_handler(cpu_state_t cpu_state, stack_state_t stack_state, uint64_t int_vec) { 
    UNUSED(cpu_state);
    UNUSED(stack_state);
    UNUSED(int_vec); 

    cli();
    com_log("exception");
    hlt();
}
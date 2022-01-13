#include "interrupt.h"
#include "com.h"
#include "kbd.h"
#include "pic.h"
#include <asm/asm.h>
#include <libmacros.h>

void general_intr_handler(cpu_state_t cpu_state) { 
    UNUSED(cpu_state);
    switch (cpu_state.int_vec) {
        case KBD_IRQ_VEC:
            kbd_intr_handler();
            break;
        default:
            hlt();
    } 
}


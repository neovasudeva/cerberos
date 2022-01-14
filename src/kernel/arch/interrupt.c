#include "interrupt.h"
#include "com.h"
#include "kbd.h"
#include "pic.h"
#include "idt.h"
#include <asm/asm.h>
#include <libmacros.h>

extern void* isr_addr_table[];

void register_intr_handler(void* handler, uint8_t irq_num) {
    isr_addr_table[irq_num] = handler;
    idt_set_descriptor(irq_num, isr_addr_table[irq_num], INT_GATE | PL0 | PRESENT);
}

void generic_intr_handler(cpu_state_t cpu_state) { 
    UNUSED(cpu_state);
    hlt();
}


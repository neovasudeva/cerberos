#include <sys/sys.h>

#include <cpu/interrupt.h>
#include <cpu/idt.h>
#include <dev/com.h>
#include <dev/kbd.h>
#include <dev/pic.h>

extern void* isr_addr_table[];

void register_intr_handler(void* handler, uint8_t irq_num) {
    isr_addr_table[irq_num] = handler;
    idt_set_descriptor(irq_num, isr_addr_table[irq_num], INT_GATE | PL0 | PRESENT);
}

void generic_intr_handler(cpu_state_t cpu_state) { 
    UNUSED(cpu_state);
    hlt();
}


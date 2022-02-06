#include <cpu/idt.h>

extern void* isr_addr_table[];
idtr_t idt_info = {};

__attribute__ ((aligned(0x10)))
idt_desc_t idt[IDT_MAX_ENTRIES] = {};

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t attributes) {
    idt_desc_t* idt_entry = &idt[vector];

    idt_entry->isr_low = ((uint64_t) isr) & 0xFFFF;
    idt_entry->seg_sel = KERNEL_CS;
    idt_entry->ist = 0;
    idt_entry->attributes = attributes;
    idt_entry->isr_mid = ((uint64_t) isr >> 16) & 0xFFFF;
    idt_entry->isr_high = ((uint64_t) isr >> 32) & 0xFFFFFFFF;
    idt_entry->reserve2 = 0;
}

void idt_init(void) {
    /* store info about idt */
    idt_info.address = (uint64_t) &idt;
    idt_info.size = sizeof(idt_desc_t) * IDT_MAX_ENTRIES - 1;
    
    /* set up all idt descriptors */
    for (int i = 0; i < IDT_MAX_ENTRIES; i++) {
        idt_set_descriptor(i, isr_addr_table[i], INT_GATE | PL0 | PRESENT);
    }

    /* load idtr */
    idt_load();
}

#pragma once

#include <stdint.h>

/* kernel code segment index in GDT */
#define KERNEL_CS               0x08

/* idt constants */
#define IDT_MAX_ENTRIES         256
#define IDT_RESERVED_ENTRIES    32

/* idt descriptor attributes */
#define INT_GATE                0x0E 
#define PL0                     0x00
#define PL3                     0x60
#define PRESENT                 0x80

/* idt descriptor */
typedef struct {
    uint16_t isr_low;
    uint16_t seg_sel; 
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserve2; 
} __attribute__ ((packed)) idt_desc_t;

/* idtr */
typedef struct {
    uint16_t size;
    uint64_t address;
} __attribute__ ((packed)) idtr_t;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t attributes);
void idt_init(void);
extern void idt_load();

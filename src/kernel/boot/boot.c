#include <stdint.h>
#include <core/macros.h>
#include <core/asm.h>
#include <log.h>

#include <boot/stivale2.h>
#include <dev/text.h>
#include <dev/com.h>
#include <dev/pic.h>
#include <dev/kbd.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <mm/pmm.h>

/*
 * entry point for kernel 
 */
void _start(struct stivale2_struct *handover) {
    UNUSED(handover);
    com_init();
    gdt_init();
    idt_init();
    pic_disable();

    /*
    int64_t out;
    asm volatile (
        "mov $0x80000008, %%rax \n\
        cpuid \n\
        mov %%rax, %0"
        : "=r" (out)
        : 
        : "%rax"
    );
    log("physical address bits: %u\n", (out & 0xFF));
    log("virtual address bits: %u\n", (out >> 8) & 0xFF);
    */

    for (;;) {
        hlt();
    }
}
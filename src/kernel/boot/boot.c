#include <stdint.h>
#include <core/macros.h>
#include <core/asm.h>

#include "stivale2.h"
#include "text.h"
#include "com.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "kbd.h"

/*
 * entry point for kernel 
 */
void _start(struct stivale2_struct *stivale2_struct) {
    UNUSED(stivale2_struct);

    com_init();
    gdt_init();
    idt_init();
    pic_disable();

    for (;;) {
        hlt();
    }
}
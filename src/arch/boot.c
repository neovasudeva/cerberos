#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"
#include "text.h"
#include "com.h"
#include "gdt.h"

#define UNUSED(expr) ((void) expr)

/*
 * entry point for kernel 
 */
void _start(struct stivale2_struct *stivale2_struct) {
    UNUSED(stivale2_struct);

    com_init();
    gdt_init();
    text_write(0, 'H', RED, GREEN);

    for (;;) {
        asm ("hlt");
    }
}
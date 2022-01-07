#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"
#include "text.h"

#define UNUSED(expr) ((void) expr)

/*
 * entry point for kernel 
 */
void _start(struct stivale2_struct *stivale2_struct) {
    UNUSED(stivale2_struct);

    text_write(0, 'H', RED, GREEN);
    text_write(1, 'e', RED, GREEN);
    text_write(2, 'l', RED, GREEN);
    text_write(3, 'l', RED, GREEN);
    text_write(4, 'o', RED, GREEN);

    // set cursor to position 10
    move_cursor(6);
    
    for (;;) {
        asm ("hlt");
    }
}
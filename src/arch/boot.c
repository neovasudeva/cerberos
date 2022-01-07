#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"
#include "text.h"
#include "com.h"

#define UNUSED(expr) ((void) expr)

/*
 * entry point for kernel 
 */
void _start(struct stivale2_struct *stivale2_struct) {
    UNUSED(stivale2_struct);

    com_init();
    com_log("hello world\n");
    com_log("how is it going today?\n");

    for (;;) {
        asm ("hlt");
    }
}
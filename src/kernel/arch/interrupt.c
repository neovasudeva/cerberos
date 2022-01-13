#include "interrupt.h"
#include "com.h"
#include "kbd.h"
#include "pic.h"
#include "text.h"
#include <asm/asm.h>
#include <libmacros.h>

void general_intr_handler(cpu_state_t cpu_state) { 
    switch (cpu_state.int_vec) {
        case KBD_IRQ_VEC:
            kbd_intr_handler(cpu_state);
            break;
        default:
            hlt();
    } 
}

static uint16_t loc = 0;
void kbd_intr_handler(cpu_state_t cpu_state) {
    UNUSED(cpu_state);
    uint8_t scan_code = inb(KBD_DATA_PORT);
    UNUSED(scan_code);

    text_write(loc, 'H', RED, GREEN);
    loc++;
    move_cursor(loc);
    send_eoi(KBD_IRQ);
}

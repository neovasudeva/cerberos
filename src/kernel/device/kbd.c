#include "kbd.h"
#include "pic.h"
#include "text.h"
#include "interrupt.h"

void kbd_init(void) {
    register_intr_handler(&wrapper_kbd_intr_handler, KBD_IRQ_VEC);
    enable_irq(KBD_IRQ);
}

void kbd_intr_handler(void) {
    inb(KBD_DATA_PORT);
    send_eoi(KBD_IRQ);
}
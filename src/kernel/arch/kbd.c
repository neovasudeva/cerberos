#include "kbd.h"
#include "pic.h"
#include "text.h"

void kbd_init(void) {
    enable_irq(KBD_IRQ);
}

static uint16_t loc = 0;
void kbd_intr_handler(void) {
    inb(KBD_DATA_PORT);

    text_write(loc, 'H', RED, GREEN);
    loc++;
    move_cursor(loc);
    send_eoi(KBD_IRQ);
}
#include <dev/kbd.h>
#include <dev/text.h>
#include <intr/apic.h>

// TODO: remove me
static char temp = 'A';

void kbd_init(void) {
    register_intr_handler(&wrapper_kbd_intr_handler, KBD_INTR_VEC);
    ioapic_quickset_irq(0, KBD_IRQ, 0, KBD_INTR_VEC);
}

void kbd_intr_handler(void) {
    inb(KBD_DATA_PORT);
    text_write(0, temp, RED, GREEN);
    temp++;
    lapic_eoi(KBD_INTR_VEC);
}
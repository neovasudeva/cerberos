#include <dev/kbd.h>
#include <dev/text.h>
#include <intr/apic.h>
#include <log.h>

// TODO: remove me
static char temp = 'A';

void kbd_init(void) {
    register_intr_handler(&wrapper_kbd_intr_handler, KBD_INTR_VEC);
    ioapic_quickset_irq(find_ioapic(KBD_IRQ), find_redtbl_entry(KBD_IRQ), bsp_lapic_id(), KBD_INTR_VEC); // TODO: remove bsp_lapic_id()
}

void kbd_intr_handler(void) {
    inb(KBD_ENC_PORT);
    text_write(0, temp, RED, GREEN);
    temp++;
    lapic_eoi(KBD_INTR_VEC);
}
#include "kbd.h"
#include "pic.h"
#include "text.h"

void kbd_init(void) {
    enable_irq(KBD_IRQ);
}

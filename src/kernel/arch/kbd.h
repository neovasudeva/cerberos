#pragma once

#include <stdint.h>

#define KBD_DATA_PORT   0x60
#define KBD_IRQ         1
#define KBD_IRQ_VEC     (KBD_IRQ + 0x20)

void kbd_init(void);
void kbd_intr_handler(void);
extern void wrapper_kbd_intr_handler(void);
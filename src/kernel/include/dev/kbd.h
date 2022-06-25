#pragma once

#include <sys/sys.h>
#include <intr/interrupt.h>

#define KBD_DATA_PORT   0x60
#define KBD_IRQ         1
#define KBD_INTR_VEC    (KBD_IRQ + INTR_DEV_OFFSET)

void kbd_init(void);
void kbd_intr_handler(void);
extern void wrapper_kbd_intr_handler(void);
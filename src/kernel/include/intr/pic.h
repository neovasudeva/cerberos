#pragma once

#include <stdint.h>

/* 
 * PIC 
 * for more information:
 * https://wiki.osdev.org/8259_PIC
 */

#define MASTER_COMMAND_PORT     0x20
#define MASTER_DATA_PORT        0x21
#define SLAVE_COMMAND_PORT      0xA0
#define SLAVE_DATA_PORT         0xA1

/* control words to initialize pic */
#define ICW1                0x11
#define ICW2_MASTER         0x20
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04
#define ICW3_SLAVE          0x02
#define ICW4                0x01

/* slave PIC attached to IRQ2 on master PIC */
#define SLAVE_LINE  0x02

/* EOI signal to send to both PICs */
#define EOI 0x20

void pic_init(void);
void pic_remap(void);
void pic_disable(void);
void pic_disable_all(void);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);
void pic_eoi(uint8_t irq);
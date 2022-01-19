#pragma once

#include <stdint.h>

/*
 * serial I/O
 * very useful debugging tool using serial communication between 
 * two computers 
 * 
 * for more information:
 * https://littleosbook.github.io/#the-serial-ports
 */

#define SERIAL_COM1                     0x3F8
#define SERIAL_COM2                     0x2F8
#define SERIAL_COM3                     0x3E8
#define SERIAL_COM4                     0x2E8

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_DIVISOR_HIGH_PORT(base)  (base + 1)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_ENABLE_DLB               0x80

void com_putc(uint16_t com, const char c);
void _com_putc(const char c);
void com_write(uint16_t com, const char* buf, uint32_t len);
void _com_write(char* buf);
void _com_init(uint16_t com, uint16_t divisor);
void com_init(void);

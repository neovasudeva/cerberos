#include "com.h"

void com_putc(uint16_t com, const char c) {
    while ((inb(SERIAL_LINE_COMMAND_PORT(com)) & 0x20) == 0) {}
    outb(SERIAL_DATA_PORT(com), c);
}

void com_write(uint16_t com, const char* buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        outb(SERIAL_DATA_PORT(com), buf[i]);
    }
}

void com_log(const char* buf) {
    int i = 0;
    uint8_t c = buf[i];
    while (c != '\0') {
        outb(SERIAL_DATA_PORT(SERIAL_COM1), c);
        i++; 
        c = buf[i];
    }
}

void _com_init(uint16_t com, uint16_t divisor) {
    /* establish baud rate */
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_ENABLE_DLB);
    outb(SERIAL_DATA_PORT(com), divisor & 0xFF);
    outb(SERIAL_INTERRUPT_PORT(com), (divisor >> 8) & 0xFF);

    /* configure line */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);

    /* configure FIFO */
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);

    /* configure modem */
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

void com_init(void) {
    _com_init(SERIAL_COM1, 3);
}
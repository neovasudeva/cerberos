#include <sys/io.h> 
#include <dev/com.h>

void com_putc(uint16_t com, const char c) {
    outb(SERIAL_DATA_PORT(com), c);
}

void __com_putc(const char c) {
    com_putc(SERIAL_COM1, c);
}

void com_write(uint16_t com, const char* buf, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        com_putc(com, buf[i]);
    }
}

void __com_puts(char* buf) {
    int i = 0;
    while (buf[i] != '\0') {
        __com_putc(buf[i]);
        i++; 
    }
}

void __com_init(uint16_t com, uint16_t divisor) {
    /* establish baud rate */
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_ENABLE_DLB);
    outb(SERIAL_DATA_PORT(com), divisor & 0xFF);
    outb(SERIAL_DIVISOR_HIGH_PORT(com), (divisor >> 8) & 0xFF);

    /* configure line */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);

    /* configure FIFO */
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);

    /* configure modem */
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

void com_init(void) {
    __com_init(SERIAL_COM1, 3);
}
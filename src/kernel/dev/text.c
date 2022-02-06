#include <dev/text.h>

void text_write(uint16_t loc, uint8_t c, uint8_t fg, uint8_t bg) {
    uint16_t* text_buffer = (uint16_t*) TEXT_ADDR;
    text_buffer[loc] = ((uint16_t) (c) | (uint16_t) (fg) << 8 | (uint16_t) (bg) << 12);
}

void move_cursor(uint16_t loc) {
    outb(TEXT_CMD_PORT, HIGH_BYTE_CMD);
    outb(TEXT_DATA_PORT, ((loc >> 8) & 0x00FF));
    outb(TEXT_CMD_PORT, LOW_BYTE_CMD);
    outb(TEXT_DATA_PORT, loc & 0x00FF);
}
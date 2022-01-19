#include <string/string.h>
#include <stdarg.h>

#include "log.h"
#include "com.h"

void print_level(log_level_t level) {
    /* print log level */
    switch (level) {
        case LOG_INFO:
            _com_write("[INFO]: ");
            break;
        case LOG_TODO: 
            _com_write("[TODO]: ");
            break;
        case LOG_ERROR:
            _com_write("[ERROR]: ");
            break;
        default:
            _com_write("[... no log level ...] ");
    }
}

/*
 * log
 *
 * This implementation is very ugly. Please clean up later
 * 
 * All integer values are assumed to be int64_t
 * %% = print '%'
 * %x = print value as hexadecimal
 * %u = print value as uint64_t
 * %d = print value as int64_t
 * %c = print value as uint8_t
 * %s = print value as string
 */
void log(log_level_t level, char* format, ...) {
    print_level(level);

    va_list args;
    va_start(args, format);
    
    /* '%' detected while parsing */
    bool perc = false;
    char* buf = format;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                if (perc) {
                    _com_putc('%');
                }
                perc = !perc;
                break;
            case 'x':
                if (perc) {
                    char temp[64];
                    memset(temp, 0, 64);
                    itoa(va_arg(args, uint64_t), temp, 16);
                    _com_write(temp);
                    perc = false;
                } else {
                    _com_putc('x');
                }
                break;
            case 'u':
                if (perc) {
                    char temp[64];
                    memset(temp, 0, 64);
                    itoa(va_arg(args, uint64_t), temp, 10);
                    _com_write(temp);
                    perc = false;
                } else {
                    _com_putc('u');
                }
                break;
            case 'd':
                if (perc) {
                    char temp[64];
                    memset(temp, 0, 64);
                    itoa(va_arg(args, int64_t), temp, 10);
                    _com_write(temp);
                    perc = false;
                } else {
                    _com_putc('d');
                }
                break;
            case 'c':
                if (perc) {
                    _com_putc((char) va_arg(args, int));
                    perc = false;
                } else {
                    _com_putc('c');
                }
                break;
            case 's':
                if (perc) {
                    _com_write((char*) va_arg(args, int*));
                    perc = false;
                } else {
                    _com_putc('s');
                }
                break;
            default:
                _com_putc(*buf);
        }

        buf++;
    } 
}
#include <core/utils/utils.h>
#include <string/string.h>
#include <stdarg.h>
#include <core/macros.h>

#include "log.h"
#include "com.h"

/* 
 * the following are handlers 
 * each prints and returns the update to perc 
 */

static inline bool parse_p(bool perc) {
    if (perc) {
        __com_putc('%');
    }
    return !perc;
}

static inline bool parse_xu(bool perc, int64_t val, char op) {
    char temp[64];
    memset(temp, 0, 64);
    
    if (perc) {
        if (op == 'x') {
            itoa(val, temp, 16, UNSIGNED);
        } else if (op == 'u') {
            itoa(val, temp, 10, UNSIGNED);
        }
        __com_write(temp);
    } else {
        __com_putc(op);
    }

    return false;
}

static inline bool parse_d(bool perc, int64_t val) {
    char temp[64];
    memset(temp, 0, 64);
    
    if (perc) {
        itoa(val, temp, 10, SIGNED);
        __com_write(temp);
    } else {
        __com_putc('d');
    }

    return false;
}

static inline bool parse_c(bool perc, char c) {
    if (perc) {
        __com_putc(c);
    } else {
        __com_putc('c');
    }

    return false;
}

static inline bool parse_s(bool perc, char* str) {
    if (perc) {
        __com_write(str);
    } else {
        __com_putc('s');
    }

    return false;
}

/* print log level */
void print_level(log_level_t level) {
    switch (level) {
        case LOG_INFO:
            __com_write("[INFO]: ");
            break;
        case LOG_TODO: 
            __com_write("[TODO]: ");
            break;
        case LOG_ERROR:
            __com_write("[ERROR]: ");
            break;
        case NONE:
            break;
        default:
            __com_write("[... no log level ...] ");
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
    void* curr_arg;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                perc = parse_p(perc);
                break;
            case 'x':
            case 'u':
                if (perc) curr_arg = (uint64_t) va_arg(args, unsigned int);
                perc = parse_xu(perc, (uint64_t) curr_arg, *buf);
                break;
            case 'd':
                if (perc) curr_arg = (int64_t) va_arg(args, int);
                perc = parse_d(perc, (int64_t) curr_arg);
                break;
            case 'c':
                if (perc) curr_arg = (int64_t) va_arg(args, int);
                perc = parse_c(perc, (char) curr_arg);
                break;
            case 's':
                if (perc) curr_arg = (int64_t) va_arg(args, int);
                perc = parse_s(perc, (char*) curr_arg);
                break;
            default:
                __com_putc(*buf);
        }

        buf++;
    } 
}
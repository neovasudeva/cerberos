#include <stdarg.h>
#include <dev/com.h>
#include <sys/sys.h>
#include <mem/mem.h>

#include "log.h"

/* 
 * the following are handlers 
 * each prints and returns the update to perc 
 */

static inline void print_x(int32_t val) {
    char buf[9];
    buf[8] = '\0';

    char* digits = "0123456789abcdef";
    for (int i = 7; i >= 0; i--) {
        buf[i] = digits[val & 0xF]; 
        val >>= 4;
    }

    __com_puts(buf);
}

static inline void print_lx(int64_t val) {
    char buf[17];
    buf[16] = '\0';

    char* digits = "0123456789abcdef";
    for (int i = 15; i >= 0; i--) {
        buf[i] = digits[val & 0xF]; 
        val >>= 4;
    }

    __com_puts(buf);
}

static inline void print_u(uint32_t val) {
    char buf[11];
    buf[10] = '\0';
    int i = 10;
    const int ascii_offset = '0';

    if (!val) {
        __com_puts(0);
        return;
    }

    while (val) {
        buf[--i] = (val % 10) + ascii_offset;
        val /= 10;
    }

    __com_puts((buf + i));
}

static inline void print_lu(uint64_t val) {
    char buf[21];
    buf[20] = '\0';
    int i = 20;
    const int ascii_offset = '0';

    if (!val) {
        __com_putc('0');
        return;
    }

    while (val) {
        buf[--i] = (val % 10) + ascii_offset;
        val /= 10;
    }

    __com_puts((buf + i));
}

static inline void print_d(int32_t val) {
    char buf[12];
    buf[11] = '\0';
    int i = 11;
    const int ascii_offset = '0';
    bool neg = val < 0;

    if (!val) {
        __com_putc('0');
        return;
    }

    if (neg) val = -val;

    while (val) {
        buf[--i] = (val % 10) + ascii_offset;
        val /= 10;
    }

    if (neg) buf[--i] = '-';

    __com_puts((buf + i));
}

static inline void print_ld(int64_t val) {
    char buf[21];
    buf[20] = '\0';
    int i = 20;
    const int ascii_offset = '0';
    bool neg = val < 0;

    if (!val) {
        __com_putc('0');
        return;
    }

    if (neg) val = -val;

    while (val) {
        buf[--i] = (val % 10) + ascii_offset;
        val /= 10;
    }

    if (neg) buf[--i] = '-';

    __com_puts((buf + i));
}

/* print log level */
void print_level(log_level_t level) {
    switch (level) {
        case INFO:
            __com_puts("[INFO]: ");
            break;
        case TODO: 
            __com_puts("[TODO]: ");
            break;
        case ERROR:
            __com_puts("[ERROR]: ");
            break;
        case NONE:
            break;
        default:
            __com_puts("[... no log level ...] ");
    }
}

/*
 * log
 *
 * When passing in numeric values to log, please use long and 
 * unsigned long suffixes (ex. 69l) or cast to int64_t or 
 * uint64_t respectively
 * 
 * %% = print '%'
 * %c = print value as char
 * %s = print value as string
 * %x = print value as 32-bit hexadecimal
 * %lx = print value as 64-bit hexadecimal
 * %u = print value as 32-bit unsigned
 * %lu = print value as 64-bit unsigned hexadecimal
 * %d = print value as 32-bit decimal
 * %ld = print value as 64-bit decimal
 * 
 * returns 0 if parsing was successful and -1 if failed
 */
int log_impl(log_level_t level, char* format, ...) {
    print_level(level);

    va_list args;
    va_start(args, format);

    char* buf = format;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                buf++;

                if (*buf == '\0')
                    return -1;

                switch (*buf) {
                    case '%':
                        __com_putc('%');
                        break;
                    case 'c':
                        __com_putc((char) va_arg(args, int));
                        break;
                    case 's':
                        __com_puts((char*) va_arg(args, char*));
                        break;
                    case 'x':
                        print_x((int32_t) va_arg(args, int32_t));
                        break;
                    case 'u':
                        print_u((uint32_t) va_arg(args, uint32_t));
                        break;
                    case 'd':
                        print_d((int32_t) va_arg(args, int32_t));
                        break;
                    case 'l':
                        buf++;

                        if (*buf == '\0')
                            return -1;

                        switch (*buf) {
                            case 'x':
                                print_lx((int64_t) va_arg(args, int64_t));
                                break;
                            case 'u':
                                print_lu((uint64_t) va_arg(args, uint64_t));
                                break;
                            case 'd':
                                print_ld((int64_t) va_arg(args, int64_t));
                                break;
                            default: 
                                return -1;
                        }
                        
                        break;

                    default:
                        return -1;
                }
                
                break;

            default:
                __com_putc(*buf);
        }

        buf++;
    } 

    return 0;
}

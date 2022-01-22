#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    LOG_INFO,
    LOG_TODO,
    LOG_ERROR,
    NONE
} log_level_t;

//bool parse_p(bool perc);
//bool parse_xud(bool perc, int64_t val, char op);
//bool parse_c(bool perc, char c);
//bool parse_s(bool perc, char* str);

void print_level(log_level_t level);
void log(log_level_t level, char* format, ...);


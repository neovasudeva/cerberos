#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef enum {
    LOG_INFO,
    LOG_TODO,
    LOG_ERROR,
    NONE
} log_level_t;

void print_level(log_level_t level);
void log(log_level_t level, char* format, ...);


#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    INFO,
    TODO,
    ERROR,
    WARNING,
    NONE
} log_level_t;

void print_level(log_level_t level);
int log_impl(log_level_t level, char* format, ...);

#define info(format, ...)       log_impl(INFO, format, ## __VA_ARGS__) 
#define todo(format, ...)       log_impl(TODO, format, ## __VA_ARGS__)
#define error(format, ...)      log_impl(ERROR, format, ## __VA_ARGS__)
#define warning(format, ...)    log_impl(WARNING, format, ## __VA_ARGS__)
#define log(format, ...)        log_impl(NONE, format, ## __VA_ARGS__)



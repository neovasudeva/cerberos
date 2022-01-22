#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SIGNED,
    UNSIGNED
} sign_t;

char* itoa(int64_t value, char* str, int64_t base, sign_t sign);
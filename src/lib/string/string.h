#pragma once

#include <stddef.h>
#include <stdbool.h>

void* memcpy(void* dest, const void* src, size_t num);
void* memmove(void* dest, const void* src, size_t num);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* src);

/* not in ANSI C */
char* itoa(int value, char* str, int base);

/* more to come ... */

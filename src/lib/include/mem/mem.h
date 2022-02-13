#pragma once

#include <stddef.h>
#include <stdbool.h>

void* memcpy(void* dest, const void* src, size_t num);
void* memmove(void* dest, const void* src, size_t num);
void memset(void* buf, int c, size_t n);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* src);

/* more to come ... */

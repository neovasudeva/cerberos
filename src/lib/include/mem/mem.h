#pragma once

#include <stddef.h>
#include <stdbool.h>

void* memcpy(void* dest, const void* src, size_t num);
void* memmove(void* dest, const void* src, size_t num);
void memset(void* buf, int c, size_t n);
int memcmp(const void* buf1, const void* buf2, size_t num);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* src);
int strncmp(const char* str1, const char* str2, size_t num);

/* more to come ... */

#pragma once

#include <cpuid.h> // gcc specific header

/* 
 * for more info:
 * https://wiki.osdev.org/CPUID
 */

/* 
 * physical address bits: eax && 0xFF
 * virtual address bits: (eax >> 8) & 0xFF
 */
static inline int cpuid_va_pa_sizes(void) {
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
    return eax;
}

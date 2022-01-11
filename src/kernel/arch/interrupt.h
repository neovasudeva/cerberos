#pragma once

#include <stdint.h>

/* manually pushed regs onto stack during interrupt */
typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
} __attribute__ ((packed)) cpu_state_t;

/* data pushed onto stack by CPU during interrupt */
typedef struct {
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
} __attribute__ ((packed)) stack_state_t;

void interrupt_handler(cpu_state_t cpu_state, stack_state_t stack_state, uint64_t int_vec); 


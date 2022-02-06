#pragma once

#include <stdint.h>

/* 
 * Port interaction functions
 */

/* 
 * inb reads a byte from a port and returns it
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    asm volatile (
        "xorb %0, %0 \n\
        inb %1, %0"
        : "=a"(data)
        : "d"(port)
        :
    );
    return data;
}

/* 
 * inw reads 2 bytes from 2 consecutive ports and returns it
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t data;
    asm volatile (
        "xorw %0, %0 \n\
        inw %1, %0"
        : "=a"(data)
        : "d"(port)
        :
    );
    return data;
}

/* 
 * inw reads 4 bytes from 4 consecutive ports and returns it
 */
static inline uint32_t inl(uint16_t port) {
    uint32_t data;
    asm volatile (
        "xorl %0, %0 \n\
        inl %1, %0"
        : "=a"(data)
        : "d"(port)
        :
    );
    return data;
}

/*
 * outb writes a byte to a port
 */
static inline void outb(uint16_t port, uint8_t data) {
    asm volatile (
        "outb %0, %1"
        :
        : "a"(data), "d"(port)
        :
    );
}

/*
 * outw writes 2 bytes to 2 consecutive ports
 */
static inline void outw(uint16_t port, uint16_t data) {
    asm volatile (
        "outw %0, %1"
        :
        : "a"(data), "d"(port)
        :
    );
}

/*
 * outl writes 4 bytes to 4 consecutive ports
 */
static inline void outl(uint16_t port, uint32_t data) {
    asm volatile (
        "outl %0, %1"
        :
        : "a"(data), "d"(port)
        :
    );
}

/*
 * io_wait throws data out to unregistered port
 * used to give time for I/O operations
 */
static inline void io_wait(void) {
    outb(0x80, 0);
}

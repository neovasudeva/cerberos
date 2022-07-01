#pragma once

#include <sys/sys.h>
#include <intr/interrupt.h>

/* interrupt information for PIT */
#define PIT_IRQ         0
#define PIT_INTR_VEC    (PIT_IRQ + INTR_DEV_OFFSET)

/* frequency dividers */
#define PIT_FREQ        ((uint64_t) 1193182)
#define PIT_DIV_1KHZ    ((uint64_t) 1193)       // 1000Hz 

/* ports for talking to PIT */
typedef enum {
    PIT_CHANNEL0_PORT = 0x40,
    PIT_CHANNEL1_PORT = 0x41,
    PIT_CHANNEL2_PORT = 0x42,
    PIT_CMD_PORT  = 0x43
} pit_port_e;

/* CMD reg macros */

/* PIT channels */
#define PIT_CHANNEL0    0b00
#define PIT_CHANNEL1    0b01
#define PIT_CHANNEL2    0b10
#define PIT_READ_BACK   0b11

/* PIT access modes */
#define PIT_LATCH_COUNT 0b00
#define PIT_LOW         0b01
#define PIT_HIGH        0b10
#define PIT_LOW_HIGH    0b11

/* PIT operating modes */
#define PIT_MODE0       0b000
#define PIT_MODE1       0b001
#define PIT_MODE2       0b010
#define PIT_MODE3       0b011
#define PIT_MODE4       0b100
#define PIT_MODE5       0b101

/* PIT BCD/Binary modes */
#define PIT_BIN_MODE    0b0
#define PIT_BCD_MODE    0b1

/* PIT CMD struct */
typedef union {
    struct {
        uint8_t bcd_bin_mode : 1;
        uint8_t operating_mode : 3;
        uint8_t access_mode : 2;
        uint8_t channel : 2;
    } __attribute__((packed));
    uint8_t raw;
} pit_cmd_t; 

void pit_init(void);
void pit_intr_handler(void);
extern void wrapper_pit_intr_handler(void);
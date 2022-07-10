#pragma once

#include <sys/sys.h>
#include <acpi/madt.h>
#include <intr/interrupt.h>

/* change me */
#define LAPIC_TIMER_IRQ         10
#define LAPIC_TIMER_INTR_VEC    (LAPIC_TIMER_IRQ + INTR_DEV_OFFSET)

/* LAPIC reg offsets */
typedef enum {
    LAPIC_ID_REG              = 0x20,
    LAPIC_VERSION_REG         = 0x30,
    LAPIC_EOI_REG             = 0xB0,
    LAPIC_SPR_INTR_VEC_REG    = 0xF0,
    LAPIC_ICR_LOW             = 0x300,
    LAPIC_ICR_HIGH            = 0x310,

    /* In-Service Register (ISR) */
    LAPIC_ISR_31_0            = 0x100,
    LAPIC_ISR_63_32           = 0x110,
    LAPIC_ISR_95_64           = 0x120,
    LAPIC_ISR_127_96          = 0x130,
    LAPIC_ISR_159_128         = 0x140,
    LAPIC_ISR_191_160         = 0x150,
    LAPIC_ISR_223_192         = 0x160,
    LAPIC_ISR_255_224         = 0x170,

    /* LVT regs */
    LAPIC_LVT_CMI_REG         = 0x2f0,
    LAPIC_LVT_TIMER_REG       = 0x320,
    LAPIC_LVT_THERMAL_REG     = 0x330,
    LAPIC_LVT_PERF_REG        = 0x340,
    LAPIC_LVT_LINT0_REG       = 0x350,
    LAPIC_LVT_LINT1_REG       = 0x360,
    LAPIC_LVT_ERROR_REG       = 0x370,

    /* LAPIC timer regs */
    LAPIC_INIT_COUNT_REG      = 0x380,
    LAPIC_CURR_COUNT_REG      = 0x390,
    LAPIC_DIVIDE_CONFIG_REG   = 0x3e0
} lapic_reg_e;

/* LVT delivery types */
#define LVT_FIXED   0b000
#define LVT_SMI     0b010
#define LVT_NMI     0b100
#define LVT_INIT    0b101
#define LVT_EXTINT  0b111

/* LVT delivery status */
#define LVT_IDLE    0b0
#define LVT_PENDING 0b1

/* LVT pin polarity */
#define LVT_HIGH    0b0
#define LVT_LOW     0b1

/* LVT trigger mode */
#define LVT_EDGE    0b0
#define LVT_LEVEL   0b1

/* LVT mask */
#define LVT_UNMASK  0b0
#define LVT_MASK    0b1

/* LVT entry struct */
typedef union {
    struct {
        uint8_t vector;
        uint8_t delivery_mode : 3;
        uint8_t reserved0 : 1;
        uint8_t deliver_status : 1;
        uint8_t pin_polarity : 1;
        uint8_t remote_irr : 1;
        uint8_t trigger_level : 1;
        uint8_t mask : 1;
        uint16_t reserved1 : 15;
    } __attribute__((packed));
    uint32_t raw;
} lvt_entry_t;

/* IPI delivery types */
#define IPI_FIXED       0b000
#define IPI_LP          0b001
#define IPI_SMI         0b010
#define IPI_NMI         0b100
#define IPI_INIT        0b101
#define IPI_START       0b110 

/* IPI delivery mode */
#define IPI_PHYS        0b0
#define IPI_LOG         0b1

/* IPI delivery status */
#define IPI_IDLE        0b0
#define IPI_PENDING     0b1

/* IPI level */
#define IPI_DEASSERT    0b0 
#define IPI_ASSERT      0b1

/* IPI trigger mode */
#define IPI_EDGE        0b0 
#define IPI_LEVEL       0b1

/* IPI destination shorthand */
#define IPI_NO_SH       0b00
#define IPI_SELF        0b01
#define IPI_ALL_IN_SELF 0b10
#define IPI_ALL_EX_SELF 0b11

/* LVT ICR reg for IPIs */
typedef union {
    struct {
        uint8_t vector;
        uint8_t delivery_mode : 3;
        uint8_t dest_mode : 1;
        uint8_t delivery_status : 1;
        uint8_t reserved0 : 1;
        uint8_t level : 1;
        uint8_t trigger_mode : 1;
        uint8_t reserved1 : 2;
        uint8_t dest_shorthand : 2;
        uint64_t reserved2 : 36;
        uint8_t destination;
    } __attribute__ ((packed));

    struct {
        uint32_t low_raw;
        uint32_t high_raw;
    } __attribute__((packed));
} ipi_t;

/* LVT timer delivery status */
#define LVT_TIMER_IDLE      0b0
#define LVT_TIMER_PENDING   0b1

/* LVT Timer mask */
#define LVT_TIMER_UNMASK    0b0
#define LVT_TIMER_MASK      0b1

/* LVT Timer mode */
#define LVT_TIMER_ONE_SHOT  0b00
#define LVT_TIMER_PERIODIC  0b01
#define LVT_TIMER_TSC       0b10

/* LVT timer register */
typedef union {
    struct {
        uint8_t vector;
        uint8_t reserved0 : 4;
        uint8_t delivery_status : 1;
        uint8_t reserved1 : 3;
        uint8_t mask : 1;
        uint8_t timer_mode : 2;
        uint16_t reserved2 : 13;
    } __attribute__ ((packed));
    uint32_t raw;
} lvt_timer_t;

/* timer divide register values */
#define LAPIC_TIMER_DIV_1   0b111
#define LAPIC_TIMER_DIV_2   0b000
#define LAPIC_TIMER_DIV_4   0b001
#define LAPIC_TIMER_DIV_8   0b010
#define LAPIC_TIMER_DIV_16  0b011
#define LAPIC_TIMER_DIV_32  0b100
#define LAPIC_TIMER_DIV_64  0b101
#define LAPIC_TIMER_DIV_128 0b110

/* Timer divide register divide set */
#define LAPIC_TIMER_DIV_SET(div_reg, val) \
    div_reg.divide01 = (val & 3);   \
    div_reg.divide2 = (val & 8);

/* Timer divide register */
typedef union {
    struct {
        uint8_t divide01 : 2;
        uint8_t zero : 1;
        uint8_t divide2 : 1;
        uint32_t reserved : 28;
    } __attribute__ ((packed));
    uint32_t raw;
} timer_divide_t;

/* lapic api */
void lapic_enable(void);
void lapic_disable(void);
void lapic_eoi(uint8_t intr_vec);

void lapic_timer(void);
void lapic_timer_intr_handler(void);
extern void wrapper_lapic_timer_intr_handler(void);

uint32_t lapic_read(lapic_reg_e reg);
void lapic_write(lapic_reg_e reg, uint32_t val);
void lapic_ipi(ipi_t ipi);
uint8_t lapic_id(void);
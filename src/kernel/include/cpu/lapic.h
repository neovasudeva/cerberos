#pragma once

#include <sys/sys.h>
#include <acpi/madt.h>

/* LAPIC reg offsets */
typedef enum {
    LAPIC_ID_REG              = 0x20,
    LAPIC_VERSION_REG         = 0x30,
    LAPIC_EOI_REG             = 0xB0,
    LAPIC_SPR_INTR_VEC_REG    = 0xF0,
    LAPIC_ICR_LOW             = 0x300,
    LAPIC_ICR_HIGH            = 0x310,

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
#define LVT_EXTINIT 0b111

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

/* lapic api */
void lapic_enable(void);
void lapic_disable(void);
void lapic_eoi(void);

uint32_t lapic_read(lapic_reg_e reg);
void lapic_write(lapic_reg_e reg, uint32_t val);
void lapic_ipi(ipi_t* ipi);
uint8_t lapic_id(void);
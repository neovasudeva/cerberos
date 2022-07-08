#pragma once

#include <sys/sys.h>
#include <intr/interrupt.h>

/* HPET misc macros */
#define HPET_REG_WIDTH   0x08
#define HPET_MIN_TIMERS  3
#define HPET_MAX_TIMERS  32
#define HPET_VALID_IRQS  32

/* interrupt macros */
#define HPET_IRQ        0
#define HPET_INTR_VEC   (HPET_IRQ + INTR_DEV_OFFSET)

/* Timer n's regs address offsets */
#define HPET_TIMER_CCR(n)     (0x100 + 0x20 * n)
#define HPET_TIMER_CVR(n)     (0x108 + 0x20 * n)
#define HPET_TIMER_FSBIRR(n)  (0x110 + 0x20 * n)

/* HPET general regs address offsets */
typedef enum {
    HPET_GCIDR  = 0x0,
    HPET_GCR    = 0x10, 
    HPET_GISR   = 0x20,
    HPET_MCR    = 0xF0
} hpet_general_reg_e;

/* General Capabilities and ID Reg (GCIDR) */
#define HPET_GCIDR_MAX_CLK_PERIOD   0x05F5E100
#define HPET_GCIDR_LEG_REPL_CAP     1
#define HPET_GCIDR_64BIT_CAP        1

typedef union {
    struct {
        uint8_t revision_id;
        uint8_t last_timer_id : 5;
        uint8_t count_size_cap : 1;
        uint8_t reserved : 1;
        uint8_t legacy_replacement_cap : 1;
        uint16_t vendor_id;
        uint32_t counter_clk_period;
    } __attribute__((packed));
    uint64_t raw;
} hpet_gcidr_t;

/* General Congifuration Register (GCR) */
#define HPET_GCR_LEG_REPL_DISABLE   0
#define HPET_GCR_LEG_REPL_ENABLE    1
#define HPET_GCR_MAIN_CNT_DISABLED  0   // timer interrupts are also disabled
#define HPET_GCR_MAIN_CNT_RUNNING   1   // timer interrupts allowed if enabled

typedef union {
    struct {
        uint8_t enable_config : 1;
        uint8_t legacy_replacement_config : 1;
        uint64_t reserved : 62;
    } __attribute__((packed));
    uint64_t raw;
} hpet_gcr_t;

/* General Interrupt Status Register (GISR) */
typedef union {
    struct {
        uint32_t intr_status;
        uint32_t reserved;
    } __attribute__((packed));
    uint64_t raw;
} hpet_gisr_t;

/* Main Counter Value Register (MCVR) */
typedef union {
    uint64_t count;
    uint64_t raw;
} hpet_mcvr_t;

/* Timer N Config and Capabilities Register (CCR) */
#define HPET_CCR_EDGE_INTR      0
#define HPET_CCR_LEVEL_INTR     1
#define HPET_CCR_INTR_DISABLE   0   // timer will still update Nth bit of GISR's intr_status
#define HPET_CCR_INTR_ENABLE    1
#define HPET_CCR_ONE_SHOT_MODE  0
#define HPET_CCR_PERIODIC_MODE  1
#define HPET_CCR_PERIODIC_CAP   1
#define HPET_CCR_64BIT_CAP      1
#define HPET_CCR_FORCE_32BIT    1
#define HPET_CCR_FSB_INTR       1
#define HPET_CCR_FSB_INTR_CAP   1
#define HPET_CCR_VAL_SET_CFG    1

typedef union {
    struct {
        uint8_t reserved0 : 1;
        uint8_t intr_type_config : 1;
        uint8_t intr_enable_config : 1;
        uint8_t mode_config : 1;
        uint8_t periodic_mode_cap : 1;
        uint8_t mode64_cap : 1;
        uint8_t value_set_config : 1;
        uint8_t reserved1 : 1;
        uint8_t mode32_config : 1;
        uint8_t intr_route_config : 5;
        uint8_t fsb_enable_config : 1;
        uint8_t fsb_intr_cap : 1;
        uint16_t reserved2;
        uint32_t intr_route_cap;
    } __attribute__((packed));
    uint64_t raw;
} hpet_ccr_t;

/* Timer N Comparator Value Register (CVR) */
typedef union {
    uint64_t comp_value;
    uint64_t raw;
} hpet_cvr_t;

/* HPET general metadata */
typedef struct {
    hpet_gcidr_t gcidr;
    uint64_t hpet_addr;
    uint64_t num_timers;
    uint64_t freq;
    uint64_t min_tick;
} hpet_general_t;

/* Timer N metadata */
typedef struct {
    hpet_ccr_t ccr;
    bool present;
    uint8_t timer_id;
    uint32_t intr_irq_cap; 
} hpet_timer_t;

bool hpet_init(void);
uint64_t hpet_read(uint64_t reg_offset);
void hpet_write(uint64_t reg_offset, uint64_t val);

void hpet_intr_handler(void);
extern void wrapper_hpet_intr_handler(void);

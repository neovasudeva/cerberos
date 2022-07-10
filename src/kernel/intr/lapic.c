#include <intr/apic.h>
#include <acpi/madt.h>
#include <log.h>

/* miscellaneous macros */
#define LAPIC_ALL               0xff
#define LAPIC_ID_SHIFT          24
#define LAPIC_EOI               1       // this is arbitrary, can be anything
#define LAPIC_REG_OFFSET        0x10
#define LAPIC_REG_DATA_WIDTH    32

/*
 * configure_nmi
 * configures NMI entry into LVT on LINT0 or LINT1
 */
static inline void configure_nmi(void) {
    for (uint64_t i = 0; i < get_madt_info()->num_lapic_nmi; i++) {
        madt_lapic_nmi_t* nmi = get_madt_info()->lapic_nmis[i];
        if (lapic_id() == nmi->acpi_processor_id || nmi->acpi_processor_id == LAPIC_ALL) {
            lvt_entry_t lvt_nmi = {
                .vector         = 2,                                        // ignored, 2 = NMI exception vector
                .delivery_mode  = LVT_NMI,
                .pin_polarity   = (nmi->flags & 2) ? LVT_LOW : LVT_HIGH,    // not completely correct: refer ACPI 6.0 Table 5-51
                .trigger_level  = (nmi->flags & 8) ? LVT_LEVEL : LVT_EDGE,  // not completely correct: refer ACPI 6.0 Table 5-51
                .mask           = LVT_UNMASK
            };

            if (nmi->lint == LINT0)         
                lapic_write(LAPIC_LVT_LINT0_REG, lvt_nmi.raw);
            else if (nmi->lint == LINT1)    
                lapic_write(LAPIC_LVT_LINT1_REG, lvt_nmi.raw);
            else                            
                panic("[lapic_enable] illegal lint value was detected: %u\n", nmi->lint);
        }
    }
}

/*
 * lapic_read
 * reads data from LAPIC register
 * @param reg : LAPIC register to read from 
 */
inline uint32_t lapic_read(lapic_reg_e reg) {
    return *((volatile uint32_t*) (get_madt_info()->lapic_paddr + reg));
}

/*
 * lapic_write
 * writes data to a LAPIC reg
 * @param reg : LAPIC register to write to
 * @param val : value to write into register 
 */
inline void lapic_write(lapic_reg_e reg, uint32_t val) {
    *((volatile uint32_t*) (get_madt_info()->lapic_paddr + reg)) = val;
}

/*
 * lapic_ipi
 * sends and IPI given fields in @param ipi
 * @param ipi : struct containing how and where IPI should be delivered
 */
inline void lapic_ipi(ipi_t ipi) {
    lapic_write(LAPIC_ICR_HIGH, ipi.high_raw);
    lapic_write(LAPIC_ICR_LOW, ipi.low_raw);
}

/* 
 * lapic_id
 * @returns the LAPIC ID for this processor
 */
inline uint8_t lapic_id(void) {
    return (uint8_t) (lapic_read(LAPIC_ID_REG) >> LAPIC_ID_SHIFT);
}

/* 
 * lapic_enable
 * enables the LAPIC for this processor
 */
void lapic_enable(void) {
    // configure LINT0/LINT1 NMI entries
    configure_nmi();

    // set bit 8 of Spurious Interrupt Vector to enable LAPIC
    uint32_t val = lapic_read(LAPIC_SPR_INTR_VEC_REG);
    val |= (1 << 8);
    lapic_write(LAPIC_SPR_INTR_VEC_REG, val);
}

/* 
 * lapic_disable
 * disables the LAPIC for this processor
 */
void lapic_disable(void) {
    // clear bit 8 of Spurious Interrupt Vector to enable LAPIC
    uint32_t val = lapic_read(LAPIC_SPR_INTR_VEC_REG);
    val &= ~(1 << 8);
    lapic_write(LAPIC_SPR_INTR_VEC_REG, val);
}

/* 
 * lapic_eoi
 * sends EOI to processor's LAPIC after handling interrupt
 * EOI is only necessary if the @param intr_vec bit of ISR is set
 * @param intr_vec : the interrupt vector of the interrupt
 */
void lapic_eoi(uint8_t intr_vec) {
    uint8_t isr_reg_offset = intr_vec / LAPIC_REG_DATA_WIDTH;    
    uint8_t isr_reg_bit = intr_vec % LAPIC_REG_DATA_WIDTH;

    uint32_t isr = lapic_read(LAPIC_ISR_31_0 + isr_reg_offset * LAPIC_REG_OFFSET);
    if (isr & (1 << isr_reg_bit)) 
        lapic_write(LAPIC_EOI_REG, LAPIC_EOI);
}

/*
 *
 */
void lapic_timer(void) {
    // set the timer interrupt vector, periodic/one-shot mode in timer LVT reg
    lvt_timer_t timer;
    timer.raw = lapic_read(LAPIC_LVT_TIMER_REG); 
    log("timer initial raw : 0x%x\n", timer.raw);
    timer.vector = LAPIC_TIMER_INTR_VEC;
    timer.timer_mode = LVT_TIMER_PERIODIC;
    lapic_write(LAPIC_LVT_TIMER_REG, timer.raw);

    // register intr handler
    register_intr_handler(&wrapper_lapic_timer_intr_handler, LAPIC_TIMER_INTR_VEC);

    // set divide reg
    timer_divide_t div;
    div.raw = lapic_read(LAPIC_DIVIDE_CONFIG_REG);
    LAPIC_TIMER_DIV_SET(div, LAPIC_TIMER_DIV_4); // arbitrary, change
    lapic_write(LAPIC_DIVIDE_CONFIG_REG, div.raw);

    // set initial count reg
    uint32_t icr = 1024;
    lapic_write(LAPIC_INIT_COUNT_REG, icr);

    // unmask timer interrupt in LVT
    timer.raw = lapic_read(LAPIC_LVT_TIMER_REG);
    timer.mask = LVT_TIMER_UNMASK;
    lapic_write(LAPIC_LVT_TIMER_REG, timer.raw);

}

void lapic_timer_intr_handler(void) {
    log("hello lapic timer !!!\n");
    lapic_eoi(LAPIC_TIMER_INTR_VEC);
}
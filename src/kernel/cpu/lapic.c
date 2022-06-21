#include <cpu/lapic.h>
#include <log.h>

#define LAPIC_ALL       0xff
#define LAPIC_ID_SHIFT  24

extern madt_info_t madt_info;

static inline void configure_nmi(void) {
    for (uint64_t i = 0; i < madt_info.num_lapic_nmi; i++) {
        lapic_nmi_t* nmi = madt_info.lapic_nmis[i];
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

inline uint32_t lapic_read(lapic_reg_e reg) {
    return *((uint32_t volatile*) (madt_info.lapic_paddr + reg));
}

inline void lapic_write(lapic_reg_e reg, uint32_t val) {
    *((uint32_t volatile*) (madt_info.lapic_paddr + reg)) = val;
}

inline void lapic_ipi(ipi_t* ipi) {
    lapic_write(LAPIC_ICR_HIGH, ipi->high_raw);
    lapic_write(LAPIC_ICR_LOW, ipi->low_raw);
}

inline uint8_t lapic_id(void) {
    return (uint8_t) (lapic_read(LAPIC_ID_REG) >> LAPIC_ID_SHIFT);
}

void lapic_enable(void) {
    // configure LINT0/LINT1 NMI entries
    configure_nmi();

    // set bit 8 of Spurious Interrupt Vector to enable LAPIC
    uint32_t val = lapic_read(LAPIC_SPR_INTR_VEC_REG);
    val |= (1 << 8);
    lapic_write(LAPIC_SPR_INTR_VEC_REG, val);
}

void lapic_disable(void) {
    // clear bit 8 of Spurious Interrupt Vector to enable LAPIC
    uint32_t val = lapic_read(LAPIC_SPR_INTR_VEC_REG);
    val &= ~(1 << 8);
    lapic_write(LAPIC_SPR_INTR_VEC_REG, val);
}

void lapic_eoi(void) {
    // FINISH ME
}
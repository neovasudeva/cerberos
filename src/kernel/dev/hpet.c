#include <dev/hpet.h>
#include <acpi/acpi.h>
#include <acpi/hpet.h>
#include <intr/apic.h>
#include <mm/mem.h>
#include <log.h>

/* misc marcos */
#define FEMTO_PER_SEC       ((uint64_t) 0x38D7EA4C68000) // = 10^15

/* HPET general info */
hpet_general_t hpet = {0};

/* HPET timer-specific info */
hpet_timer_t timers[HPET_MAX_TIMERS] = {0};

static bool verify_hpet_checksum(hpet_header_t* hpet) {
    uint8_t checksum = 0;
    uint8_t* ptr = (uint8_t*) hpet;

    for (size_t i = 0; i < hpet->sdt_hdr.length; i++) {
        checksum += *ptr;
        ptr++;
    }

    return checksum == 0;
}

static inline bool timer_irq_capable(uint8_t timer_id, uint8_t irq) {
    return timers[timer_id].intr_irq_cap & (1 << irq);
}

bool hpet_init(void) {
    // find HPET table among ACPI tables
    hpet_header_t* hpet_hdr = (hpet_header_t*) acpi_find_table("HPET");
    if (hpet_hdr == NULL) {
        error("[hpet_init] hpet header not found in ACPI tables.\n");
        return false;
    }

    // verify header
    if (!verify_hpet_checksum(hpet_hdr)) {
        error("[hpet_init] HPET table failed checksum check.\n");
        return false;
    }

    // initialize HPET metadata
    hpet.hpet_addr = P2V(hpet_hdr->address_info.address);
    hpet.gcidr.raw = hpet_read(HPET_GCIDR);
    hpet.num_timers = hpet.gcidr.last_timer_id + 1;
    hpet.freq = FEMTO_PER_SEC / hpet.gcidr.counter_clk_period;
    hpet.min_tick = hpet_hdr->minimum_tick;

    // verify HPET supports legacy mode
    if (!hpet.gcidr.legacy_replacement_cap) {
        error("[hpet_init] HPET does not support legacy interrupt routing.\n");
        return false;
    }

    // load timers' metadata
    for (uint8_t i = 0; i < hpet.num_timers; i++) {
        timers[i].ccr.raw = hpet_read(HPET_TIMER_CCR(i));
        timers[i].present = true;
        timers[i].timer_id = i;
        timers[i].intr_irq_cap = timers[i].ccr.raw >> HPET_VALID_IRQS;
    }

    // register interrupt handler in ioapic irq 0
    register_intr_handler(&wrapper_hpet_intr_handler, HPET_INTR_VEC);
    ioapic_quickset_irq(find_ioapic(HPET_IRQ), find_redtbl_entry(HPET_IRQ), bsp_lapic_id(), HPET_INTR_VEC);

    // clear counter
    hpet_write(HPET_MCR, 0);

    // setup timer 0 in legacy mode
    hpet_ccr_t timer_ccr;
    timer_ccr.raw = hpet_read(HPET_TIMER_CCR(0));
    timer_ccr.intr_type_config = HPET_CCR_EDGE_INTR;
    timer_ccr.mode_config = HPET_CCR_PERIODIC_MODE;
    timer_ccr.value_set_config = HPET_CCR_VAL_SET_CFG;
    timer_ccr.intr_enable_config = HPET_CCR_INTR_ENABLE;
    hpet_write(HPET_TIMER_CCR(0), timer_ccr.raw);

    hpet_cvr_t timer_cvr;
    timer_cvr.raw = hpet.freq / 1000; // 1000 is arbitrary. represents number of counts per interrupt
    hpet_write(HPET_TIMER_CVR(0), timer_cvr.raw);

    // enable legacy routing and start main counter
    hpet_gcr_t hpet_gcr;
    hpet_gcr.raw = hpet_read(HPET_GCR);
    hpet_gcr.legacy_replacement_config = HPET_GCR_LEG_REPL_ENABLE;
    hpet_gcr.enable_config = HPET_GCR_MAIN_CNT_RUNNING;
    hpet_write(HPET_GCR, hpet_gcr.raw);

    return true;
}

inline uint64_t hpet_read(uint64_t reg_offset) {    
    return *((volatile uint64_t*) (hpet.hpet_addr + reg_offset));
}

inline void hpet_write(uint64_t reg_offset, uint64_t val) {
    *((volatile uint64_t*) (hpet.hpet_addr + reg_offset)) = val;
}

void hpet_intr_handler(void) {
    //log("HPET WORKING !!!!!\n");
    lapic_eoi(HPET_INTR_VEC);
}

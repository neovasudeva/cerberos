#include <cpu/ioapic.h>
#include <mem/mem.h>
#include <log.h>

#define REDTBL_NUM_ENTRIES          24
#define MAX_IOAPICS                 16  // IO APIC id is only 4 bits

#define is_registered(ioapic_id)    (ioapic_id < MAX_IOAPICS && ioapics[ioapic_id].present == true)

/* metadata about all IO APICs present */
ioapic_t ioapics[MAX_IOAPICS] = {0};

inline void ioapic_register(madt_ioapic_t* ioapic) {
    uint8_t id = ioapic->io_apic_id;

    // setup IO APIC metadata
    ioapics[id].present = true;
    ioapics[id].ioapic_id = id;
    ioapics[id].ioregsel = (volatile uint32_t*) ((uint64_t) ioapic->io_apic_addr);
    ioapics[id].iowin = (volatile uint32_t*) ((uint64_t) (ioapic->io_apic_addr + IOWIN));
    ioapics[id].gsi_base = ioapic->gsi_base;
    ioapics[id].redtbl_entries = ((ioapic_ver_reg_t) (ioapic_read(id, IOAPICVER))).max_redtbl_entry + 1;
}

inline uint32_t ioapic_read(uint8_t id, uint8_t reg) {
    if (!is_registered(id)) {
        error("[ioapic_read] io apic read either with invalid id %u or io apic is not present\n", id);
        return 0;
    }

    *ioapics[id].ioregsel = reg;
    return *ioapics[id].iowin;
}

inline void ioapic_write(uint8_t id, uint8_t reg, uint32_t val) {
    if (!is_registered(id)) {
        error("[ioapic_write] io apic read either with invalid id %u or io apic is not present\n", id);
        return;
    }

    *ioapics[id].ioregsel = reg;
    *ioapics[id].iowin = val;
}

// TODO: revamp, make more general
void ioapic_set_irq(uint8_t ioapic_id, uint8_t irq, uint8_t apic_id, uint8_t vector) {
    if (!is_registered(ioapic_id)) {
        error("[ioapic_set_irq] io apic read either with invalid id %u or io apic is not present\n", ioapic_id);
        return;
    }

    redtbl_entry_t entry;
    uint32_t high = ioapic_read(ioapic_id, IOREDTBL_REG_HIGH(irq));
    uint32_t low = ioapic_read(ioapic_id, IOREDTBL_REG_LOW(irq));
    entry.high_raw = high;
    entry.low_raw = low;

    entry.destination = apic_id & 0xf;
    entry.interrupt_mask = IOAPIC_UNMASK;
    entry.dest_mode = IOAPIC_PHYS_MODE;
    entry.delivery_mode = IOAPIC_FIXED;
    entry.vector = vector;

    ioapic_write(ioapic_id, IOREDTBL_REG_HIGH(irq), entry.high_raw);
    ioapic_write(ioapic_id, IOREDTBL_REG_LOW(irq), entry.low_raw);
}




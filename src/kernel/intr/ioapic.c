#include <intr/apic.h>
#include <intr/interrupt.h>
#include <mem/mem.h>
#include <log.h>

/* miscellaneous macros */
#define REDTBL_NUM_ENTRIES          24
#define MAX_IOAPICS                 16  // IO APIC id is only 4 bits

#define is_registered(ioapic_id)    (ioapic_id < MAX_IOAPICS && ioapics[ioapic_id].present == true)

/* metadata about all IO APICs present */
ioapic_t ioapics[MAX_IOAPICS] = {0};

/* static utility functions for registering interrupt source overrides from MADT */
static uint8_t ioapic_gsi_pin_polarity(uint8_t flags) {
    return (flags & 2) ? IOAPIC_LOW : IOAPIC_HIGH;
}

static uint8_t ioapic_gsi_trigger_mode(uint8_t flags) {
    return (flags & 8) ? IOAPIC_LEVEL : IOAPIC_EDGE;
}

/* 
 * ioapic_register 
 * registers an IO APIC into the ioapics array
 * @param ioapic : IO APIC info from MADT to register
 */
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

/* 
 * ioapic_read
 * reads an IO APIC register 
 * @param id : id of IO APIC
 * @param reg : IO APIC register to read 
 * @returns data in the registered
 */
inline uint32_t ioapic_read(uint8_t id, uint8_t reg) {
    if (!is_registered(id)) {
        error("[ioapic_read] io apic read either with invalid id %u or io apic is not present\n", id);
        return 0;
    }

    *ioapics[id].ioregsel = reg;
    return *ioapics[id].iowin;
}

/* 
 * ioapic_write
 * write an IO APIC register 
 * @param id : id of IO APIC
 * @param reg : IO APIC register to read 
 * @param val : value to write into the register
 */
inline void ioapic_write(uint8_t id, uint8_t reg, uint32_t val) {
    if (!is_registered(id)) {
        error("[ioapic_write] io apic read either with invalid id %u or io apic is not present\n", id);
        return;
    }

    *ioapics[id].ioregsel = reg;
    *ioapics[id].iowin = val;
}

/* 
 * ioapic_set_gsi
 * registers an interrupt source override as specified in the MADT
 * refer to 5.2.12.5 in ACPI 6.0 docs
 * @param override : pointer to interrupt source override entry in MADT 
 */
void ioapic_set_gsi(madt_intr_src_override_t* override) {
    // irq_src = isa irq
    // gsi = mapped to "gsi" redtbl entry 

    // find ioapic with appropriate gsi_base
    uint8_t ioapic_id = (uint8_t) -1;
    for (uint8_t i = 0; i < MAX_IOAPICS; i++) {
        if (ioapics[i].present && ioapics[i].gsi_base < override->gsi) 
            ioapic_id = i;
    }

    if (ioapic_id >= MAX_IOAPICS) {
        error("[ioapic_set_gsi] no io apic with low enough GSI was found. Interrupt source override GSI: %u\n", override->gsi);
        return;
    }

    // add entry to ioapic's redtbl
    redtbl_entry_t entry;
    entry.vector = INTR_DEV_OFFSET + override->irq_src;
    entry.delivery_mode = IOAPIC_FIXED;
    entry.dest_mode = IOAPIC_PHYS_MODE;
    entry.pin_polarity = ioapic_gsi_pin_polarity(override->flags);
    entry.trigger_mode = ioapic_gsi_trigger_mode(override->flags);
    entry.interrupt_mask = IOAPIC_MASK;     // leave it to device startup routine to unmask
    entry.destination = bsp_lapic_id();     // TODO: change in future
    ioapic_set_irq(ioapic_id, override->irq_src, entry);
}

/*
 * ioapic_set_irq 
 * register an IRQ in a redirection of the IO APIC with @param ioapic_id
 * @param ioapic_id : IO APIC to store redirection table entry in
 * @param irq : IRQ to register
 * @param entry : redirectino table entry to store
 */
void ioapic_set_irq(uint8_t ioapic_id, uint8_t irq, redtbl_entry_t entry) {
    // verify ioapic is registered
    if (!is_registered(ioapic_id)) {
        error("[ioapic_set_irq] io apic read either with invalid id %u or io apic is not present\n", ioapic_id);
        return;
    }

    ioapic_write(ioapic_id, IOREDTBL_REG_HIGH(irq), entry.high_raw);
    ioapic_write(ioapic_id, IOREDTBL_REG_LOW(irq), entry.low_raw);
}

/* 
 * ioapic_quickset_irq
 * shortcut version of ioapic_set_irq with predefined redirection table fields
 * @param ioapic_id : IO APIC to store redirection table entry in
 * @param irq : IRQ to register
 * @param lapic_id : LAPIC to delivery interrupt to
 * @param vector : interrupt vector to deliver to processor
 */
void ioapic_quickset_irq(uint8_t ioapic_id, uint8_t irq, uint8_t lapic_id, uint8_t vector) {
    // verify ioapic is registered
    if (!is_registered(ioapic_id)) {
        error("[ioapic_quickset_irq] io apic read either with invalid id %u or io apic is not present\n", ioapic_id);
        return;
    }

    redtbl_entry_t entry;
    uint32_t high = ioapic_read(ioapic_id, IOREDTBL_REG_HIGH(irq));
    uint32_t low = ioapic_read(ioapic_id, IOREDTBL_REG_LOW(irq));
    entry.high_raw = high;
    entry.low_raw = low;

    entry.destination = lapic_id & 0xf;
    entry.interrupt_mask = IOAPIC_UNMASK;
    entry.dest_mode = IOAPIC_PHYS_MODE;
    entry.delivery_mode = IOAPIC_FIXED;
    entry.vector = vector;

    ioapic_set_irq(ioapic_id, irq, entry);
}




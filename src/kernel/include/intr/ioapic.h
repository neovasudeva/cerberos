#pragma once

#include <sys/sys.h>
#include <acpi/madt.h>

/* IO APIC register select and window regs */
#define IOREGSEL                0x00
#define IOWIN                   0x10

/* IO APIC reg indices */
#define IOAPICID                0x0
#define IOAPICVER               0x1
#define IOAPICARB               0x2

/* IO APIC redirection table indices*/
#define IOREDTBL                    0x10 
#define IOREDTBL_REG_LOW(entry)     (IOREDTBL + 2 * entry)
#define IOREDTBL_REG_HIGH(entry)    (IOREDTBL + 2 * entry + 1)

/* IO APIC ID reg */
typedef union {
    struct {
        uint32_t reserved0 : 24;
        uint8_t ioapic_id : 4;
        uint8_t reserved1 : 4;
    } __attribute__((packed));

    uint32_t raw;
} ioapic_id_reg_t;

/* IO APIC VER reg */
typedef union {
    struct {
        uint8_t apic_version;
        uint8_t reserved0;
        uint8_t max_redtbl_entry;
        uint8_t reserved1;
    } __attribute__((packed));

    uint32_t raw;
} ioapic_ver_reg_t;

/* IO APIC ARB reg */
typedef union {
    struct {
        uint32_t reserved0 : 24;
        uint8_t arb_id : 4;
        uint8_t reserved1 : 4;
    } __attribute__((packed));

    uint32_t raw;
} ioapic_arb_reg_t;

/* IO APIC delivery types */
#define IOAPIC_FIXED            0b000
#define IOAPIC_LOW_PRIOR        0b001
#define IOAPIC_SMI              0b010
#define IOAPIC_NMI              0b100
#define IOAPIC_INIT             0b101
#define IOAPIC_EXTINT           0b111

/* IO APIC delivery status */
#define IOAPIC_IDLE             0b0
#define IOAPIC_PENDING          0b1

/* IO APIC destination modes */
#define IOAPIC_PHYS_MODE        0b0
#define IOAPIC_LOG_MODE         0b1

/* IO APIC pin polarity */
#define IOAPIC_HIGH             0b0
#define IOAPIC_LOW              0b1

/* IO APIC trigger mode */
#define IOAPIC_EDGE             0b0
#define IOAPIC_LEVEL            0b1

/* IO APIC interrupt masking */
#define IOAPIC_UNMASK           0b0
#define IOAPIC_MASK             0b1


/* IO APIC redirection table */
typedef union {
    struct {
        uint8_t vector;
        uint8_t delivery_mode : 3;
        uint8_t dest_mode : 1;
        uint8_t delivery_status : 1;
        uint8_t pin_polarity : 1;
        uint8_t remote_irr : 1;
        uint8_t trigger_mode : 1;
        uint8_t interrupt_mask : 1;
        uint64_t reserved : 39;
        uint8_t destination;
    } __attribute__((packed));

    struct {
        uint32_t low_raw;
        uint32_t high_raw;
    } __attribute__((packed));
} redtbl_entry_t;

/* IO APIC info struct */
typedef struct {
    bool present;
    uint8_t ioapic_id;
    volatile uint32_t* ioregsel;
    volatile uint32_t* iowin;
    uint32_t gsi_base;
    uint32_t redtbl_entries;
} ioapic_t;

void ioapic_register(madt_ioapic_t* ioapic);
uint32_t ioapic_read(uint8_t id, uint8_t reg);
void ioapic_write(uint8_t id, uint8_t reg, uint32_t val);
void ioapic_set_gsi(madt_intr_src_override_t* override);
void ioapic_set_irq(uint8_t ioapic_id, uint8_t irq, redtbl_entry_t entry);
void ioapic_quickset_irq(uint8_t ioapic_id, uint8_t irq, uint8_t apic_id, uint8_t vector);
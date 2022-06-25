#pragma once

#include <sys/sys.h>
#include <acpi/sdt.h>

#define MADT_LAPIC                  ((uint8_t) 0)
#define MADT_IOAPIC                 ((uint8_t) 1)
#define MADT_INTR_SRC_OVERRIDE      ((uint8_t) 2)
#define MADT_APIC_NMI_SRC           ((uint8_t) 3)
#define MADT_LAPIC_NMI              ((uint8_t) 4)
#define MADT_LAPIC_ADDR_OVERRIDE    ((uint8_t) 5)
#define MADT_LX2APIC                ((uint8_t) 9)

#define MADT_OFFSET                 ((uint8_t) 0x2c)

#define LINT0                       0
#define LINT1                       1

/* MADT */
typedef struct {
    sdt_header_t sdt_hdr;
    uint32_t lapic_addr;
    uint32_t flags;
    uint8_t data[];
} __attribute__((packed)) madt_t;

/* headers for entries in MADT */
typedef struct {
    uint8_t entry_type;
    uint8_t length;
} __attribute__ ((packed)) madt_header_t;

/* MADT entry structs */
typedef struct {
    madt_header_t madt_hdr;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__ ((packed)) madt_lapic_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t gsi_base;
} __attribute__ ((packed)) madt_ioapic_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
} __attribute__ ((packed)) madt_intr_src_override_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t nmi_src;
    uint8_t reserved;
    uint16_t flags;
    uint32_t gsi;
} __attribute__ ((packed)) madt_apic_nmi_src_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t acpi_processor_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__ ((packed)) madt_lapic_nmi_t;

typedef struct {
    madt_header_t madt_hdr;
    uint16_t reserved;
    uint64_t lapic_paddr;
} __attribute__ ((packed)) madt_lapic_addr_override_t;

typedef struct {
    madt_header_t madt_hdr;
    uint16_t reserved;
    uint32_t lx2apic_id;
    uint32_t flags;
    uint32_t acpi_id;
} __attribute__ ((packed)) madt_lx2apic_t;

/* MADT info struct */
typedef struct {
    uint64_t num_lapic;
    madt_lapic_t** lapics;

    uint64_t num_io_apic;
    madt_ioapic_t** io_apics;

    uint64_t num_intr_src_override;
    madt_intr_src_override_t** intr_src_overrides;

    uint64_t num_apic_nmi_src;
    madt_apic_nmi_src_t** nmi_srcs;

    uint64_t num_lapic_nmi;
    madt_lapic_nmi_t** lapic_nmis;

    madt_lapic_addr_override_t* lapic_addr_override;

    uint64_t num_lx2apic;
    madt_lx2apic_t** lx2apics;

    uint64_t lapic_paddr;
} madt_info_t;

madt_info_t* get_madt_info(void);
void parse_madt(sdt_header_t* madt_hdr);
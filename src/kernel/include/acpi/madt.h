#pragma once

#include <sys/sys.h>
#include <acpi/sdt.h>

#define PROCESSOR_LAPIC             ((uint8_t) 0)
#define IO_APIC                     ((uint8_t) 1)
#define INTERRUPT_SRC_OVERRIDE      ((uint8_t) 2)
#define APIC_NMI_SRC                ((uint8_t) 3)
#define LAPIC_NMI                   ((uint8_t) 4)
#define LAPIC_ADDR_OVERRIDE         ((uint8_t) 5)
#define LX2APIC                     ((uint8_t) 9)

#define MADT_OFFSET                 ((uint8_t) 0x2c)

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
} __attribute__ ((packed)) processor_lapic_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_addr;
    uint32_t gsi_base;
} __attribute__ ((packed)) io_apic_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
} __attribute__ ((packed)) interrupt_src_override_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t nmi_src;
    uint8_t reserved;
    uint16_t flags;
    uint32_t gsi;
} __attribute__ ((packed)) apic_nmi_src_t;

typedef struct {
    madt_header_t madt_hdr;
    uint8_t acpi_processor_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__ ((packed)) lapic_nmi_t;

typedef struct {
    madt_header_t madt_hdr;
    uint16_t reserved;
    uint64_t lapic_paddr;
} __attribute__ ((packed)) lapic_addr_override_t;

typedef struct {
    madt_header_t madt_hdr;
    uint16_t reserved;
    uint32_t lx2apic_id;
    uint32_t flags;
    uint32_t acpi_id;
} __attribute__ ((packed)) lx2apic_t;

void parse_madt(sdt_header_t* madt_hdr);
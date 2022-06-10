#pragma once

#include <sys/sys.h>
#include <stivale/stivale2.h>
#include <acpi/sdt.h>

#define ACPI_V1             0
#define ACPI_V2             2
#define RSDP_DESC_SIZE_V1   20
#define RSDP_DESC_SIZE_V2   36

/* rsdp descriptor */
typedef struct {
    // ACPI version 1
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;

    // ACPI version 2+
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp_t;

/* rsdt/xsdt struct */
typedef struct {
    sdt_header_t sdt_hdr;
    uint32_t table_ptr[];  
} __attribute__((packed)) rsdt_t;

typedef struct {
    sdt_header_t sdt_hdr;
    uint64_t table_ptr[];
} __attribute__((packed)) xsdt_t;

void acpi_init(struct stivale2_struct* handover);
sdt_header_t* acpi_find_table(sdt_header_t* root_sdt_hdr, uint8_t acpi_version, char* table);
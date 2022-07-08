#pragma once

#include <sys/sys.h>
#include <acpi/sdt.h>

struct address_structure {
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved1;
    uint64_t address;
} __attribute__ ((packed));

typedef struct {
    sdt_header_t sdt_hdr;
    uint8_t hardware_rev_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved0 : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    struct address_structure address_info;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} __attribute__ ((packed)) hpet_header_t;

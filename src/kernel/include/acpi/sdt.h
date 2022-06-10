#pragma once

#include <sys/sys.h>

/* generic sdt header */
typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oemid[6];
    char oemid_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) sdt_header_t;

bool verify_sdt_checksum(sdt_header_t* sdt_hdr);
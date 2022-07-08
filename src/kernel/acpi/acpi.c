#include <acpi/acpi.h>
#include <acpi/madt.h>
#include <mm/mem.h>
#include <boot/stivale2.h>
#include <mem/mem.h>
#include <log.h>

/* RSDT/XSDT */
static rsdt_t rsdt = {
    .sdt_hdr = NULL,
    .acpi_version = ACPI_V1,
    .table_ptr = NULL
};

/* 
 * verify_rsdp_checksum
 * verifies RSDP checksum given @param rsdp
 * @param rsdp : pointer to RSDP
 */
static bool verify_rsdp_checksum(rsdp_t* rsdp) {
    uint8_t checksum = 0;
    uint8_t* ptr = (uint8_t*) rsdp;

    for (size_t i = 0; i < RSDP_DESC_SIZE_V1; i++) {
        checksum += *ptr;
        ptr++;
    }

    bool valid_v1 = checksum == 0;

    if (rsdp->revision == ACPI_V1) 
        return valid_v1;

    for (size_t i = 0; i < RSDP_DESC_SIZE_V2 - RSDP_DESC_SIZE_V1; i++) {
        checksum += *ptr;
        ptr++;
    }

    return valid_v1 && (checksum == 0);
}

/* 
 * get_rsdt  
 * returns rsdt struct containing metadata of RSDT/XSDT
 * @returns RSDT metadata struct
 */
rsdt_t* get_rsdt(void) {
    return &rsdt;
}

/*
 * acpi_find_table
 * parses RSDT/XSDT and finds signature passed via @param table
 * @param table : 4 character signature of table to look for
 * @returns pointer to SDT header of table or NULL if not found
 */
sdt_header_t* acpi_find_table(char* table) {
    // verify root sdt_hdr is not NULL
    if (rsdt.sdt_hdr == NULL) {
        warning("[acpi_find_table] sdt_hdr is NULL\n");
        return NULL;
    }

    // verify ptr to rsdt/xsdt ptr tables is valid
    if (rsdt.table_ptr == NULL) {
        warning("[acpi_find_table] sdt_hdr is NULL\n");
        return NULL;
    }

    // verify table is string of length 4
    if (strlen(table) != 4) {
        warning("[acpi_find_table] table string is not of length 4: %s\n", table);
        return NULL;
    }

    // verify ACPI version
    if (rsdt.acpi_version != ACPI_V1 && rsdt.acpi_version != ACPI_V2) {
        warning("[acpi_find_table] acpi_version is not valid: %d\n", rsdt.acpi_version);
        return NULL;
    }

    // find table (varies for ACPI version 1 and 2+)
    size_t rsdt_entries = 0;
    if (rsdt.acpi_version == ACPI_V1) {
        // rsdt
        rsdt_entries = (rsdt.sdt_hdr->length - sizeof(sdt_header_t)) / sizeof(uint32_t);
        uint32_t* table_ptr = (uint32_t*) rsdt.table_ptr;

        for (size_t i = 0; i < rsdt_entries; i++) {
            sdt_header_t* sdt_hdr = (sdt_header_t*) ((uint64_t) table_ptr[i]);

            if (!strncmp(table, sdt_hdr->signature, 4)) 
                return sdt_hdr;
        }

    } else if (rsdt.acpi_version == ACPI_V2) {
        // xsdt
        rsdt_entries = (rsdt.sdt_hdr->length - sizeof(sdt_header_t)) / sizeof(uint64_t);
        uint64_t* table_ptr = (uint64_t*) rsdt.table_ptr;

        for (size_t i = 0; i < rsdt_entries; i++) {
            sdt_header_t* sdt_hdr = (sdt_header_t*) table_ptr[i];

            if (!strncmp(table, sdt_hdr->signature, 4)) 
                return sdt_hdr;
        }
    }

    warning("[acpi_find_table] table was not found\n");
    return NULL;
}

/*
 * acpi_init
 * finds ACPI tables and parses MADT
 * @param handover : bootloader handover struct
 */
void acpi_init(struct stivale2_struct* handover) {
    struct stivale2_struct_tag_rsdp* rsdp_tag = (struct stivale2_struct_tag_rsdp*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_RSDP_ID);
    if (rsdp_tag == NULL)
        panic("RSDP tag not found");

    rsdp_t* rsdp = (rsdp_t*) rsdp_tag->rsdp;
    if (!verify_rsdp_checksum(rsdp)) 
        panic("RSDP checksum could not be verified");

    log("[acpi_init] rsdp revision: %u\n", rsdp->revision);

    rsdt.sdt_hdr = (sdt_header_t*) P2V(rsdp->rsdt_address);
    rsdt.acpi_version = rsdp->revision;
    rsdt.table_ptr = (void*) (rsdt.sdt_hdr + 1);
    
    if (!verify_sdt_checksum(rsdt.sdt_hdr))
        panic("RSDT/XSDT checksum could not be verified");

    // find madt
    sdt_header_t* madt_hdr = acpi_find_table("APIC");
    if (madt_hdr == NULL) 
        panic("MADT table was not found");
    
    madt_hdr = (sdt_header_t*) P2V((paddr_t) madt_hdr);

    // parse madt
    parse_madt(madt_hdr);
}
#include <acpi/acpi.h>
#include <boot/stivale2.h>
#include <mm/mem.h>
#include <mem/mem.h>
#include <log.h>

static bool verify_rsdp_checksum(rsdp_t* rsdp) {
    uint8_t checksum = 0;
    uint8_t* ptr = (uint8_t*) rsdp;

    for (size_t i = 0; i < RSDP_DESC_SIZE_V1; i++) {
        checksum += *ptr;
        ptr++;
    }

    bool valid_v1 = (checksum & 0xff) == 0;

    if (rsdp->revision == ACPI_V1) 
        return valid_v1;

    for (size_t i = 0; i < RSDP_DESC_SIZE_V2 - RSDP_DESC_SIZE_V1; i++) {
        checksum += *ptr;
        ptr++;
    }

    return valid_v1 && ((checksum & 0xff) == 0);
}

// sdt_hdr = hdr of root table
sdt_header_t* acpi_find_table(sdt_header_t* root_sdt_hdr, uint8_t acpi_version, char* table) {
    // verify sdt_hdr is not NULL
    if (root_sdt_hdr == NULL) {
        warning("[acpi_find_table] sdt_hdr is NULL\n");
        return NULL;
    }

    // verify table is string of length 4
    if (strlen(table) != 4) {
        warning("[acpi_find_table] table string is not of length 4: %s\n", table);
        return NULL;
    }

    // find table (varies for ACPI version 1 and 2+)
    if (acpi_version == ACPI_V1) {
        rsdt_t* rsdt = (rsdt_t*) root_sdt_hdr;
        const size_t rsdt_entries = (rsdt->sdt_hdr.length - sizeof(sdt_header_t)) / sizeof(uint32_t);
        
        for (size_t i = 0; i < rsdt_entries; i++) {
            sdt_header_t* sdt_hdr = (sdt_header_t*) ((uint64_t) (rsdt->table_ptr[i]));
            if (!strncmp(table, sdt_hdr->signature, 4)) 
                return sdt_hdr;
        }

    } else if (acpi_version == ACPI_V2) {
        xsdt_t* xsdt = (xsdt_t*) root_sdt_hdr;
        const size_t xsdt_entries = (xsdt->sdt_hdr.length - sizeof(sdt_header_t)) / sizeof(uint64_t);

        for (size_t i = 0; i < xsdt_entries; i++) {
            sdt_header_t* sdt_hdr = (sdt_header_t*) xsdt->table_ptr[i];
            if (!strncmp(table, sdt_hdr->signature, 4)) 
                return sdt_hdr;
        }

    } else {
        warning("[acpi_find_table] invalid acpi version was passed in: %u\n", acpi_version);
        return NULL;
    }

    warning("[acpi_find_table] table was not found\n");
    return NULL;
}

void acpi_init(struct stivale2_struct* handover) {
    struct stivale2_struct_tag_rsdp* rsdp_tag = (struct stivale2_struct_tag_rsdp*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_RSDP_ID);
    if (rsdp_tag == NULL)
        panic("RSDP tag not found");

    rsdp_t* rsdp = (rsdp_t*) rsdp_tag->rsdp;
    if (!verify_rsdp_checksum(rsdp)) 
        panic("RSDP checksum could not be verified");

    log("rsdp revision: %u\n", rsdp->revision);

    if (rsdp->revision == ACPI_V1) {
        // ACPI version 1 w/ RSDT is used
        info("[acpi_init] ACPI version 1 used. Work still to do ...\n");

        // parse rsdt
        rsdt_t* rsdt = (rsdt_t*) P2V(rsdp->rsdt_address);
        if (!verify_sdt_checksum(&rsdt->sdt_hdr)) 
            panic("RSDT checksum could not be verified");

        // find madt
        sdt_header_t* madt_hdr = acpi_find_table(&rsdt->sdt_hdr, rsdp->revision, "APIC");
        if (madt_hdr == NULL) 
            panic("MADT table was not found");
        
        madt_hdr = (sdt_header_t*) P2V((paddr_t) madt_hdr);
        log("madt_hdr signature: %s\n", madt_hdr->signature); 

        // parse madt

        
    } else {
        // ACPI version 2 w/ XSDT is used
        log("[acpi_init] ACPI version 2 used. Work still to do ...\n");

        // get xsdt and verify checksum

        // get xsdt num entries

    }

}
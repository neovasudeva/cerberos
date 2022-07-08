#include <acpi/madt.h>
#include <mm/kheap.h>
#include <intr/ioapic.h>
#include <intr/lapic.h>
#include <log.h>

#define MAX_ENTRY_TYPE  10

/* coalesced info from MADT */
madt_info_t madt_info = {
    .num_lapic                  = 0,
    .lapics                     = NULL,
    .num_io_apic                = 0,
    .io_apics                   = NULL,
    .num_intr_src_override      = 0,
    .intr_src_overrides         = NULL,
    .num_apic_nmi_src           = 0,
    .nmi_srcs                   = NULL,
    .num_lapic_nmi              = 0,
    .lapic_nmis                 = NULL,
    .lapic_addr_override        = NULL,
    .num_lx2apic                = 0,
    .lx2apics                   = NULL,
    .lapic_paddr                = 0
};

/* 
 * reset_madt_info
 * resets all fields in madt_info struct
 */
static void reset_madt_info(void) {
    // free old lists
    if (madt_info.lapics)
        kfree(madt_info.lapics);
    if (madt_info.io_apics)
        kfree(madt_info.io_apics);
    if (madt_info.intr_src_overrides)
        kfree(madt_info.intr_src_overrides);
    if (madt_info.nmi_srcs)
        kfree(madt_info.nmi_srcs);
    if (madt_info.lapic_nmis)
        kfree(madt_info.lapic_nmis);
    if (madt_info.lx2apics)
        kfree(madt_info.lx2apics);

    // reset counts and pointers
    madt_info.num_lapic                  = 0;
    madt_info.lapics                     = NULL;
    madt_info.num_io_apic                = 0;
    madt_info.io_apics                   = NULL;
    madt_info.num_intr_src_override      = 0;
    madt_info.intr_src_overrides         = NULL;
    madt_info.num_apic_nmi_src           = 0;
    madt_info.nmi_srcs                   = NULL;
    madt_info.num_lapic_nmi              = 0;
    madt_info.lapic_nmis                 = NULL;
    madt_info.lapic_addr_override        = NULL;
    madt_info.num_lx2apic                = 0;
    madt_info.lx2apics                   = NULL;
    madt_info.lapic_paddr                = 0;
}

static bool verify_madt_checksum(madt_header_t* madt) {
    uint8_t checksum = 0;
    uint8_t* ptr = (uint8_t*) madt;

    for (size_t i = 0; i < madt->sdt_hdr.length; i++) {
        checksum += *ptr;
        ptr++;
    }

    return (checksum == 0);
}

/* 
 * get_madt_info
 * @returns pointer to madt_info struct
 */
madt_info_t* get_madt_info(void) {
    return &madt_info;
}

/* 
 * parse_madt
 * parses MADT and fills in fields of madt_info struct
 * @param madt_hdr : pointer to header of MADT 
 */
void parse_madt(sdt_header_t* madt_hdr) {
    madt_header_t* madt = (madt_header_t*) madt_hdr;
    if (!verify_madt_checksum(madt)) {
        error("[parse_madt] MADT table failed checksum check.\n");
        return;
    }

    // clear current MADT info struct
    reset_madt_info();

    // set lapic physical address
    madt_info.lapic_paddr = (uint64_t) madt->lapic_addr;

    // parse madt entries
    log("---------------------------\n");
    uint8_t i = 0; 
    while (i < madt->sdt_hdr.length - MADT_OFFSET) {
        madt_entry_header_t* entry_hdr = (madt_entry_header_t*) &madt->data[i];

        switch (entry_hdr->entry_type) {
            case MADT_LAPIC:
                madt_lapic_t* entry0 = (madt_lapic_t*) entry_hdr;
                log("[parse_madt] MADT_LAPIC:\nacpi_processor_id: %u, apic_id: %u\n", entry0->acpi_processor_id, entry0->apic_id);
                madt_info.num_lapic++;
                break;
            case MADT_IOAPIC:
                madt_ioapic_t* entry1 = (madt_ioapic_t*) entry_hdr;
                log("[parse_madt] MADT_IOAPIC:\nio_apic_id: %u, io_apic_addr: 0x%x, gsi_base: %u\n", entry1->io_apic_id, entry1->io_apic_addr, entry1->gsi_base);
                madt_info.num_io_apic++;
                break;
            case MADT_INTR_SRC_OVERRIDE:
                madt_intr_src_override_t* entry2 = (madt_intr_src_override_t*) entry_hdr;
                log("[parse_madt] MADT_INTR_SRC_OVERRIDE:\nbus_src: %u, irq_src: %u, gsi: %u, flags: 0x%x\n", entry2->bus_src, entry2->irq_src, entry2->gsi, entry2->flags);
                madt_info.num_intr_src_override++;
                break;
            case MADT_APIC_NMI_SRC:
                madt_apic_nmi_src_t* entry3 = (madt_apic_nmi_src_t*) entry_hdr;
                log("[parse_madt] MADT_APIC_NMI_SRC:\nnmi_src: %u, flags: %u, gsi: %u\n", entry3->nmi_src, entry3->flags, entry3->gsi);
                madt_info.num_apic_nmi_src++;
                break;
            case MADT_LAPIC_NMI:
                madt_lapic_nmi_t* entry4 = (madt_lapic_nmi_t*) entry_hdr;
                log("[parse_madt] MADT_LAPIC_NMI:\nacpi_processor_id: 0x%x, flags: %u, lint: %u\n", entry4->acpi_processor_id, entry4->flags, entry4->lint);
                madt_info.num_lapic_nmi++;
                break;
            case MADT_LAPIC_ADDR_OVERRIDE:
                if (madt_info.lapic_addr_override != NULL) 
                    panic("MADT contains more than one lapic address override entry.\n");

                madt_info.lapic_addr_override = (madt_lapic_addr_override_t*) entry_hdr;
                log("[parse_madt] MADT_LAPIC_ADDR_OVERRIDE:\nlapic_paddr: 0x%lx\n", madt_info.lapic_addr_override->lapic_paddr);
                break;
            case MADT_LX2APIC:
                madt_lx2apic_t* entry9 = (madt_lx2apic_t*) entry_hdr;
                log("[parse_madt] MADT_LX2APIC:\nlx2apic_id: %u, flags: %u, acpi_id: %u\n", entry9->lx2apic_id, entry9->flags, entry9->acpi_id);
                madt_info.num_lx2apic++;
                break;
            default:
                warning("[parse_madt] unknown entry type was detected: %u\n", entry_hdr->entry_type);
        }
        log("---------------------------\n");

        i += entry_hdr->length;
    }

    // build madt_info struct
    if (madt_info.num_lapic)
        madt_info.lapics = (madt_lapic_t**) kmalloc(madt_info.num_lapic * sizeof(madt_lapic_t*));
    if (madt_info.num_io_apic)
        madt_info.io_apics = (madt_ioapic_t**) kmalloc(madt_info.num_io_apic * sizeof(madt_ioapic_t*));
    if (madt_info.num_intr_src_override)
        madt_info.intr_src_overrides = (madt_intr_src_override_t**) kmalloc(madt_info.num_intr_src_override * sizeof(madt_intr_src_override_t*));
    if (madt_info.num_apic_nmi_src)
        madt_info.nmi_srcs = (madt_apic_nmi_src_t**) kmalloc(madt_info.num_apic_nmi_src * sizeof(madt_apic_nmi_src_t*));
    if (madt_info.num_lapic_nmi)
        madt_info.lapic_nmis = (madt_lapic_nmi_t**) kmalloc(madt_info.num_lapic_nmi * sizeof(madt_lapic_nmi_t*));
    if (madt_info.num_lx2apic)
        madt_info.lx2apics = (madt_lx2apic_t**) kmalloc(madt_info.num_lx2apic * sizeof(madt_lx2apic_t*));

    i = 0; 
    uint64_t entry_counters[MAX_ENTRY_TYPE] = {0};
    while (i < madt->sdt_hdr.length - MADT_OFFSET) {
        madt_entry_header_t* entry_hdr = (madt_entry_header_t*) &madt->data[i];

        switch (entry_hdr->entry_type) {
            case MADT_LAPIC:
                madt_info.lapics[entry_counters[MADT_LAPIC]++] = (madt_lapic_t*) entry_hdr;
                break;
            case MADT_IOAPIC:
                madt_info.io_apics[entry_counters[MADT_IOAPIC]++] = (madt_ioapic_t*) entry_hdr;
                break;
            case MADT_INTR_SRC_OVERRIDE:
                madt_info.intr_src_overrides[entry_counters[MADT_INTR_SRC_OVERRIDE]++] = (madt_intr_src_override_t*) entry_hdr;
                break;
            case MADT_APIC_NMI_SRC:
                madt_info.nmi_srcs[entry_counters[MADT_APIC_NMI_SRC]++] = (madt_apic_nmi_src_t*) entry_hdr;
                break;
            case MADT_LAPIC_NMI:
                madt_info.lapic_nmis[entry_counters[MADT_LAPIC_NMI]++] = (madt_lapic_nmi_t*) entry_hdr;
                break;
            case MADT_LAPIC_ADDR_OVERRIDE:
                madt_info.lapic_paddr = madt_info.lapic_addr_override->lapic_paddr;
                break;
            case MADT_LX2APIC:
                madt_info.lx2apics[entry_counters[MADT_LX2APIC]++] = (madt_lx2apic_t*) entry_hdr;
                break;
            default:
                warning("[parse_madt] unknown entry type was detected: %u\n", entry_hdr->entry_type);
        }

        i += entry_hdr->length;
    }
}
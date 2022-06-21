#include <acpi/madt.h>
#include <mm/kheap.h>
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

void parse_madt(sdt_header_t* madt_hdr) {
    madt_t* madt = (madt_t*) madt_hdr;
    info("[parse_madt] MADT information:\n");
    log("---------------------------\n");
    log("lapic address: 0x%lx\n", madt->lapic_addr);
    log("madt length: %u\n", madt->sdt_hdr.length);

    // clear current MADT info struct
    reset_madt_info();

    // set lapic physical address
    madt_info.lapic_paddr = (uint64_t) madt->lapic_addr;

    // parse madt entries
    log("---------------------------\n");
    uint8_t i = 0; 
    while (i < madt->sdt_hdr.length - MADT_OFFSET) {
        madt_header_t* entry_hdr = (madt_header_t*) &madt->data[i];

        switch (entry_hdr->entry_type) {
            case PROCESSOR_LAPIC:
                processor_lapic_t* entry0 = (processor_lapic_t*) entry_hdr;
                log("PROCESSOR LAPIC:\nacpi_processor_id: %u, apic_id: %u\n", entry0->acpi_processor_id, entry0->apic_id);
                madt_info.num_lapic++;
                break;
            case IO_APIC:
                io_apic_t* entry1 = (io_apic_t*) entry_hdr;
                log("IO_APIC:\nio_apic_id: %u, io_apic_addr: 0x%x, gsi_base: %u\n", entry1->io_apic_id, entry1->io_apic_addr, entry1->gsi_base);
                madt_info.num_io_apic++;
                break;
            case INTERRUPT_SRC_OVERRIDE:
                interrupt_src_override_t* entry2 = (interrupt_src_override_t*) entry_hdr;
                log("INTERRUPT_SRC_OVERRIDE:\nbus_src: %u, irq_src: %u, gsi: %u, flags: %u\n", entry2->bus_src, entry2->irq_src, entry2->gsi, entry2->flags);
                madt_info.num_intr_src_override++;
                break;
            case APIC_NMI_SRC:
                apic_nmi_src_t* entry3 = (apic_nmi_src_t*) entry_hdr;
                log("APIC_NMI_SRC:\nnmi_src: %u, flags: %u, gsi: %u\n", entry3->nmi_src, entry3->flags, entry3->gsi);
                madt_info.num_apic_nmi_src++;
                break;
            case LAPIC_NMI:
                lapic_nmi_t* entry4 = (lapic_nmi_t*) entry_hdr;
                log("LAPIC_NMI:\nacpi_processor_id: 0x%x, flags: %u, lint: %u\n", entry4->acpi_processor_id, entry4->flags, entry4->lint);
                madt_info.num_lapic_nmi++;
                break;
            case LAPIC_ADDR_OVERRIDE:
                if (madt_info.lapic_addr_override != NULL) 
                    panic("MADT contains more than one lapic address override entry.\n");

                madt_info.lapic_addr_override = (lapic_addr_override_t*) entry_hdr;
                log("LAPIC_ADDR_OVERRIDE:\nlapic_paddr: 0x%lx\n", madt_info.lapic_addr_override->lapic_paddr);
                break;
            case LX2APIC:
                lx2apic_t* entry9 = (lx2apic_t*) entry_hdr;
                log("LX2APIC:\nlx2apic_id: %u, flags: %u, acpi_id: %u\n", entry9->lx2apic_id, entry9->flags, entry9->acpi_id);
                madt_info.num_lx2apic++;
                break;
            default:
                warning("[parse_madt] unknown entry type was detected: %u\n", entry_hdr->entry_type);
        }
        log("---------------------------\n");

        i += entry_hdr->length;
    }

    /*
    log("num_lapic: %lu, num_io_apic: %lu, num_intr_src_override: %lu, num_apic_nmi_src: %lu, num_lapic_nmi: %lu, num_lx2apic: %lu\n", 
        madt_info.num_lapic, madt_info.num_io_apic, madt_info.num_intr_src_override, madt_info.num_apic_nmi_src, 
        madt_info.num_lapic_nmi, madt_info.num_lx2apic
    );
    */

    // build madt_info struct
    if (madt_info.num_lapic)
        madt_info.lapics = (processor_lapic_t**) kmalloc(madt_info.num_lapic * sizeof(processor_lapic_t*));
    if (madt_info.num_io_apic)
        madt_info.io_apics = (io_apic_t**) kmalloc(madt_info.num_io_apic * sizeof(io_apic_t*));
    if (madt_info.num_intr_src_override)
        madt_info.intr_src_overrides = (interrupt_src_override_t**) kmalloc(madt_info.num_intr_src_override * sizeof(interrupt_src_override_t*));
    if (madt_info.num_apic_nmi_src)
        madt_info.nmi_srcs = (apic_nmi_src_t**) kmalloc(madt_info.num_apic_nmi_src * sizeof(apic_nmi_src_t*));
    if (madt_info.num_lapic_nmi)
        madt_info.lapic_nmis = (lapic_nmi_t**) kmalloc(madt_info.num_lapic_nmi * sizeof(lapic_nmi_t*));
    if (madt_info.num_lx2apic)
        madt_info.lx2apics = (lx2apic_t**) kmalloc(madt_info.num_lx2apic * sizeof(lx2apic_t*));

    i = 0; 
    uint64_t entry_counters[MAX_ENTRY_TYPE] = {0};
    while (i < madt->sdt_hdr.length - MADT_OFFSET) {
        madt_header_t* entry_hdr = (madt_header_t*) &madt->data[i];

        switch (entry_hdr->entry_type) {
            case PROCESSOR_LAPIC:
                madt_info.lapics[entry_counters[PROCESSOR_LAPIC]++] = (processor_lapic_t*) entry_hdr;
                break;
            case IO_APIC:
                madt_info.io_apics[entry_counters[IO_APIC]++] = (io_apic_t*) entry_hdr;
                break;
            case INTERRUPT_SRC_OVERRIDE:
                madt_info.intr_src_overrides[entry_counters[INTERRUPT_SRC_OVERRIDE]++] = (interrupt_src_override_t*) entry_hdr;
                break;
            case APIC_NMI_SRC:
                madt_info.nmi_srcs[entry_counters[APIC_NMI_SRC]++] = (apic_nmi_src_t*) entry_hdr;
                break;
            case LAPIC_NMI:
                madt_info.lapic_nmis[entry_counters[LAPIC_NMI]++] = (lapic_nmi_t*) entry_hdr;
                break;
            case LAPIC_ADDR_OVERRIDE:
                madt_info.lapic_paddr = madt_info.lapic_addr_override->lapic_paddr;
                break;
            case LX2APIC:
                madt_info.lx2apics[entry_counters[LX2APIC]++] = (lx2apic_t*) entry_hdr;
                break;
            default:
                warning("[parse_madt] unknown entry type was detected: %u\n", entry_hdr->entry_type);
        }

        i += entry_hdr->length;
    }
}
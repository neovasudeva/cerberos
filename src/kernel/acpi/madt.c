#include <acpi/madt.h>
#include <log.h>

void parse_madt(sdt_header_t* madt_hdr) {
    madt_t* madt = (madt_t*) madt_hdr;
    log("signature: %s\n", madt->sdt_hdr.signature);
    log("lapic address: 0x%lx\n", madt->lapic_addr);
    log("madt length: %u\n", madt->sdt_hdr.length);

    // parse madt entries
    log("---------------------------\n");
    uint8_t i = 0; 
    while (i < madt->sdt_hdr.length - MADT_OFFSET) {
        madt_header_t* entry_hdr = (madt_header_t*) &madt->data[i];

        switch (entry_hdr->entry_type) {
            case PROCESSOR_LAPIC:
                processor_lapic_t* entry = (processor_lapic_t*) entry_hdr;
                log("PROCESSOR LAPIC:\nacpi_processor_id: %u, apic_id: %u\n", entry->acpi_processor_id, entry->apic_id);
                break;
            case IO_APIC:
                io_apic_t* entry2 = (io_apic_t*) entry_hdr;
                log("IO_APIC:\nio_apic_id: %u, io_apic_addr: 0x%x, gsi_base: %u\n", entry2->io_apic_id, entry2->io_apic_addr, entry2->gsi_base);
                break;
            case INTERRUPT_SRC_OVERRIDE:
                break;
            case APIC_NMI_SRC:
                break;
            case LAPIC_NMI:
                lapic_nmi_t* entry3 = (lapic_nmi_t*) entry_hdr;
                log("LAPIC NMI:\nacpi_processor_id: 0x%x, flags: %u, lint: %u\n", entry3->acpi_processor_id, entry3->flags, entry3->lint);
                break;
            case LAPIC_ADDR_OVERRIDE:
                break;
            case LX2APIC:
                break;
            default:
                warning("[parse_madt] unknown entry type was detected: %u\n", entry_hdr->entry_type);
        }
        log("---------------------------\n");

        i += entry_hdr->length;
    }
    
}
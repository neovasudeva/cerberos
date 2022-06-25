#include <intr/apic.h>
#include <acpi/madt.h>
#include <log.h>

/* bsp lapic id */
static uint32_t __bsp_lapic_id = 0;
static bool bsp_lapic_id_valid = false;

/* IO APIC setup flag */
static bool io_apic_valid = false;

/*
 * apic_init
 * Initializes everything APIC related: BSP LAPIC ID, IO APICs (if they haven't been 
 * started already), and enables the local APIC of the processor. apic_init is meant to 
 * be processor independent and can be used to start APICs when running on any processor
 * @param handover : bootloader handover
 */ 
void apic_init(struct stivale2_struct* handover) {
    // setup bsp_lapic_id if necessary
    if (!bsp_lapic_id_valid) {
        struct stivale2_struct_tag_smp* smp_info = (struct stivale2_struct_tag_smp*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_SMP_ID);
        if (smp_info == NULL)
            panic("[apic_init] smp struct from bootloader could not be found\n");

        __bsp_lapic_id = smp_info->bsp_lapic_id;
        bsp_lapic_id_valid = true;
    }

    // setup IO APICs if necessary
    if (!io_apic_valid) {
        log("[apic_init] enabling io apics ...\n");
        madt_info_t* madt_info = get_madt_info();

        // register IO APICs
        for (uint64_t i = 0; i < madt_info->num_io_apic; i++) 
            ioapic_register(madt_info->io_apics[i]);

        // register interrupt source overrides
        for (uint64_t i = 0; i < madt_info->num_intr_src_override; i++) 
            ioapic_set_gsi(madt_info->intr_src_overrides[i]);

        io_apic_valid = true;
    }

    // enable local APIC of current processor
    log("[apic_init] enabling local apic with id %u ... \n", lapic_id());
    lapic_enable();
}

/*
 * bsp_lapic_id
 * @returns APIC ID of BSP
 */
uint32_t bsp_lapic_id(void) {
    if (!bsp_lapic_id_valid) {
        warning("[bsp_lapic_id] woops! looks like the bsp_lapic_id hasn't been setup yet :(\n");
        return 0;
    }

    return __bsp_lapic_id;
}
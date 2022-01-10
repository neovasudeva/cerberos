#include "gdt.h"

static tss_t tss = {0};
gdt_t gdt = {};
gdt_info_t gdt_info = {};

seg_descriptor_t gdt_create_entry(uint32_t limit, uint32_t base, uint8_t access, uint8_t flags) {
    seg_descriptor_t entry = {
        .limit_low = (uint16_t) (limit & 0xFFFF),
        .base_low = (uint16_t) (base & 0xFFFF),
        .base_mid = (uint8_t) ((base >> 16) & 0xFF),
        .access = access,
        .limit_high = ((limit >> 16) && 0x0F),
        .flags = flags,
        .base_high = (uint8_t) ((base >> 24) && 0xFF)
    };
    return entry;
}

tss_descriptor_t gdt_create_tss_entry(tss_t* tss_ptr) {
    uint64_t limit = sizeof(tss_t);
    uint64_t base = (uint64_t) tss_ptr;
    tss_descriptor_t entry = {
        .limit_15_0 = (uint16_t) (limit & 0xFFFF),
        .base_15_0 = (uint16_t) (base & 0xFFFF),  
        .base_23_16 = (uint8_t) ((base >> 16) & 0xFF),
        .type = 0b1001,
        .zeroes1 = 0b0,
        .dpl = 0b00,
        .present = 0b1,
        .limit_19_16 = ((limit >> 16) & 0x0F),
        .avl = 0b0,
        .zeroes2 = 0b00,
        .granularity = 0b0,
        .base_31_24 = (uint8_t) ((base >> 24) & 0xFF),
        .base_63_32 = (uint32_t) ((base >> 32) && 0xFFFFFFFF),
        .reserve1 = 0,
        .zeroes3 = 0b00000,
        .reserve2 = 0
    };
    return entry;
}

void gdt_init(void) {
    /* 
     * fill gdt as specified in 
     * https://stackoverflow.com/questions/23978486/far-jump-in-gdt-in-bootloader
     */
    gdt.gdt_entries[0] = gdt_create_entry(0, 0, 0, 0);
    gdt.gdt_entries[1] = gdt_create_entry(0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_SEG_SYS | GDT_ACCESS_EXEC | GDT_ACCESS_RW, GDT_FLAGS_LONG_MODE);
    gdt.gdt_entries[2] = gdt_create_entry(0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_SEG_SYS | GDT_ACCESS_RW, 0);
    gdt.gdt_entries[3] = gdt_create_entry(0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_PL3 | GDT_ACCESS_SEG_SYS | GDT_ACCESS_EXEC | GDT_ACCESS_RW, GDT_FLAGS_LONG_MODE);
    gdt.gdt_entries[4] = gdt_create_entry(0, 0, GDT_ACCESS_PRESENT | GDT_ACCESS_PL3 | GDT_ACCESS_SEG_SYS | GDT_ACCESS_RW, 0);

    gdt.tss_descrip = gdt_create_tss_entry(&tss);

    gdt_info.address = (uint64_t) &gdt;
    gdt_info.size = sizeof(gdt_t) - 1;

    gdt_load();
}
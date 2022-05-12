#pragma once

#include <boot/stivale2.h>
#include <sys/sys.h>
#include <log.h>

#define MEM_USABLE          1
#define MEM_RESERVED        2
#define MEM_ACPI_RECLAIM    3
#define MEM_ACPI_NVS        4
#define MEM_BAD             5
#define MEM_BOOT_RECLAIM    0x1000
#define MEM_KERNEL          0x1001
#define MEM_FRAMEBUFFER     0x1002

/* static helper functions */
static inline void print_memmap(struct stivale2_struct_tag_memmap* mmap_info) {
    info("Printing memory map ...\n");

    uint64_t entries = mmap_info->entries;
    struct stivale2_mmap_entry* memmap = mmap_info->memmap;

    for (uint64_t i = 0; i < entries; i++) { 
        log("%d: ", i);
        switch (memmap[i].type) {
            case MEM_USABLE:
                log("usable: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_RESERVED:
                log("reserved: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_ACPI_RECLAIM:
                log("acpi reclaimbale: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_ACPI_NVS:
                log("acpi nvs: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_BAD:
                log("bad mem: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_BOOT_RECLAIM:
                log("bootloader reclaimable: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_KERNEL:
                log("kernel modules: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case MEM_FRAMEBUFFER:
                log("framebuffer: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            default:
                error("memmap has unregistered section\n");
        }
    }
}

static inline paddr_t get_mem_size(struct stivale2_struct_tag_memmap* stivale2_memmap) {
    struct stivale2_mmap_entry last = stivale2_memmap->memmap[stivale2_memmap->entries - 1];
    return last.base + last.length;
}

static inline struct stivale2_struct_tag_memmap* get_memmap(struct stivale2_struct* handover) {
    struct stivale2_struct_tag_memmap* memmap;
    memmap = stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    if (memmap == NULL) 
        return NULL;

    return memmap;
}

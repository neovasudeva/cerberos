#pragma once

#include <boot/stivale2.h>
#include <stdint.h>
#include <log.h>

/* fix me */
typedef struct stivale2_struct              stivale2_struct;
typedef struct stivale2_struct_tag_memmap   memmap_t;
typedef struct stivale2_mmap_entry          memmap_entry_t;

/* static helper functions */

static void print_memmap(memmap_t* mmap_info) {
    info("Printing memory map ...\n");

    uint64_t entries = mmap_info->entries;
    memmap_entry_t* memmap = mmap_info->memmap;

    for (uint64_t i = 0; i < entries; i++) { 
        log("%d: ", i);
        switch (memmap[i].type) {
            case 1:
                log("usable: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 2:
                log("reserved: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 3:
                log("acpi reclaimbale: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 4:
                log("acpi nvs: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 5:
                log("bad mem: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 0x1000:
                log("bootloader reclaimable: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 0x1001:
                log("kernel modules: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            case 0x1002:
                log("framebuffer: 0x%lx - 0x%lx\n", memmap[i].base, memmap[i].base + memmap[i].length - 1);
                break;
            default:
                error("memmap has unregistered section\n");
        }
    }
}

static inline memmap_t* get_memmap(stivale2_struct* handover) {
    memmap_t* memmap;
    memmap = stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    if (memmap == NULL) 
        return NULL;

    return memmap;
}

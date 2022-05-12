#pragma once

#include <mm/mem.h>
#include <mm/memmap.h>
#include <ds/bitmap.h>
#include <sys/sys.h>

#define PMM_USED    1
#define PMM_FREE    0

typedef enum {
    PMM_ZONE_DMA,
    PMM_ZONE_NORMAL,
    PMM_NUM_ZONES
} zone_e;

typedef struct {
    size_t mem_total;
    size_t mem_free;
    size_t mem_used;
    paddr_t offset;
    bitmap_t bitmap;
    size_t first_free_idx;
    zone_e zone;
} zone_t;

/* pmm api */
paddr_t pmm_alloc(zone_e zone, size_t size);
void pmm_free(paddr_t addr, size_t size);
void pmm_init(struct stivale2_struct* handover);

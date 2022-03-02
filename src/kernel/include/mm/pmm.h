#pragma once

#include <mm/memmap.h>
#include <sys/sys.h>

typedef uint8_t zone_t;

#define PMM_ZONE_DMA    0
#define PMM_ZONE_NORMAL 1
#define PMM_NUM_ZONES   2
#define PMM_PAGE_SIZE   (4 * KiB)

/* pmm api */
paddr_t pmm_alloc(zone_t zone, size_t size);
void pmm_free(paddr_t addr, size_t size);
void pmm_init(boot_info_t* handover);

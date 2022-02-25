#pragma once

#include <sys/sys.h>
#include <mm/pmm.h>
#include <log.h>

#define BUDDY_MAX_ORDER 10

/* minimum and maximum block sizes */
#define BUDDY_MIN_BS    (4 * KiB)
#define BUDDY_MAX_BS    (BUDDY_MIN_BS << BUDDY_MAX_ORDER)

/* zones */
#define BUDDY_ZONE_DMA      0
#define BUDDY_ZONE_NORMAL   1
#define BUDDY_NUM_ZONES     2

/* buddy allocator api */
void buddy_init(uint64_t total_mem);
paddr_t buddy_alloc(uint8_t zone, size_t size);
void buddy_free(paddr_t paddr, size_t size);
void buddy_set_unusable(paddr_t paddr, size_t size);

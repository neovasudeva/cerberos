#include <mem/mem.h>
#include <ds/bitmap.h>
#include <mm/pmm.h>

/* static area for allocating buddy system structures */
static uint8_t alloc_area[1 * MiB];
static uint8_t* alloc_ptr = alloc_area;

static inline uint8_t* alloc(size_t size, size_t align) {
    uint8_t* ret = ALIGN_UP((uint64_t) alloc_ptr, align);
    memset(ret, 0, size);
    alloc_ptr = ret + size;
    return ret;
}

/* memory usage stats */
static uint64_t kmem_used = 0;
static uint64_t kmem_free = 0;
static uint64_t kmem_total = 0;

/* pmm bitmaps */
/* NOTE: offsets must be 4KiB aligned */
static bitmap_t pmm_bitmap[PMM_NUM_ZONES];
static paddr_t zone_offset[PMM_NUM_ZONES] = {
    [PMM_ZONE_DMA] = 0,           // DMA: 0 MiB - 16 MiB 
    [PMM_ZONE_NORMAL] = 16 * MiB  // NORMAL: 16 MiB - end of memory
};

/* 
 * allocate physical page frames 
 * @param zone : zone to allocate in 
 * @param size : num pages to allocate 
 */
paddr_t pmm_alloc(zone_t zone, size_t size) {
    size_t first_free_idx = pmm_bitmap[zone].first_free_idx;
    //for (; first_free_idx < pmm_bitmap[zone].size; first_free_idx++) {
    //    
    //}
    UNUSED(first_free_idx);
    UNUSED(size);

    return -1;
}

void pmm_free(paddr_t addr, size_t size) {
    UNUSED(addr);
    UNUSED(size);
}

/* 
 * set region as unused 
 * @param zone : zone to allocate in
 * @param bit : bit within bitmap to start clearing
 * @param size : num bits to clearing 
 */
static void pmm_set_unused(zone_t zone, size_t bit, size_t size) {
    bitmap_clear_range(&pmm_bitmap[zone], bit, size);
}

void pmm_init(boot_info_t* handover) {
    // get memory map information from bootloader
    memmap_t* memmap = get_memmap(handover);
    if (memmap == NULL)
        panic("pmm_init could not find memory map from bootloader.");

    // TODO: remove me
    print_memmap(memmap);

    // verify at least 16 MiB of physical memory is present
    size_t mem_size = get_mem_size(memmap);
    if (mem_size < (16 * MiB))
        panic("pmm_init found less than 16 MiB of memory.");
    
    // initialize physical memory bookkeeping
    kmem_total = mem_size;
    kmem_used = mem_size;
    kmem_free = 0;

    // initialize zone bitmaps
    for (int zone = 0; zone < PMM_NUM_ZONES; zone++) {
        if (zone == PMM_NUM_ZONES - 1) {
            pmm_bitmap[zone].size = (mem_size - zone_offset[zone]) / PMM_PAGE_SIZE;
            pmm_bitmap[zone].first_free_idx = (size_t) -1;
        } else {
            pmm_bitmap[zone].size = (zone_offset[zone + 1] - zone_offset[zone]) / PMM_PAGE_SIZE;
            pmm_bitmap[zone].first_free_idx = (size_t) -1;
        }

        // set everything to reserved
        pmm_bitmap[zone].data = alloc(bitmap_size_bytes(&pmm_bitmap[zone]), 8);
        bitmap_set_range(&pmm_bitmap[zone], 0, pmm_bitmap[zone].size);
    }

    // use bootloader handover information to set unused regions as usable
    zone_t curr_zone = PMM_ZONE_DMA;
    for (uint64_t i = 0; i < memmap->entries; i++) {
        memmap_entry_t entry = memmap->memmap[i];
        if (entry.type == MEM_USABLE) {
            paddr_t entry_start = (paddr_t) ALIGN_UP(entry.base, 4 * KiB);                    // inclusive 
            paddr_t entry_end = (paddr_t) ALIGN_DOWN(entry.base + entry.length, 4 * KiB);     // exclusive

            // find each zone the entry crosses over
            paddr_t curr_zone_start = zone_offset[curr_zone];
            paddr_t curr_zone_end = (curr_zone + 1 == PMM_NUM_ZONES) ? mem_size : zone_offset[curr_zone + 1];

            // move curr_zone to where the entry starts
            while (curr_zone_end < entry_start) {
                curr_zone++;
                curr_zone_start = zone_offset[curr_zone];
                curr_zone_end = (curr_zone + 1 == PMM_NUM_ZONES) ? mem_size : zone_offset[curr_zone + 1];
            }

            while (true) {
                size_t idx = (entry_start - curr_zone_start) / PMM_PAGE_SIZE;
                size_t size = (MIN(entry_end, curr_zone_end) - entry_start) / PMM_PAGE_SIZE;
                pmm_set_unused(curr_zone, idx, size);

                if (idx < pmm_bitmap[curr_zone].first_free_idx)
                    pmm_bitmap[curr_zone].first_free_idx = idx;

                // entry is within current zone
                if (entry_end <= curr_zone_end)
                    break;
                
                // entry bleeds into next zone
                entry_start = MIN(curr_zone_end, entry_end);
                curr_zone++;
                curr_zone_start = (curr_zone + 1 > PMM_NUM_ZONES) ? mem_size : zone_offset[curr_zone];
                curr_zone_end = (curr_zone + 1 >= PMM_NUM_ZONES) ? mem_size : zone_offset[curr_zone + 1];
            }

            // update physical memory bookkeeping
            kmem_used -= entry.length;
            kmem_free += entry.length;
        }
    }

    log("end\n");
}


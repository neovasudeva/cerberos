#include <mem/mem.h>
#include <mm/pmm.h>

/* static area for allocating buddy system structures */
static uint8_t alloc_area[1 * MiB];
static uint8_t* alloc_ptr = alloc_area;

static inline uint8_t* alloc(size_t size, size_t align) {
    uint8_t* ret = (uint8_t*) ALIGN_UP((uint64_t) alloc_ptr, align);
    memset(ret, 0, size);
    alloc_ptr = ret + size;
    return ret;
}

/* memory bookkeeping */
static size_t mem_size = 0;

/* zone information */
/* NOTE: offsets must be 4KiB aligned */
static zone_t mem_zone[PMM_NUM_ZONES] = {
    // DMA: 0 MiB - 16 MiB
    [PMM_ZONE_DMA] = {
        .mem_total = (size_t) -1,
        .mem_free = (size_t) -1,
        .mem_used = (size_t) -1,
        .offset = 0,
        .bitmap = { .size = (size_t) -1, .data = NULL },
        .first_free_idx = (size_t) -1,
        .zone = PMM_ZONE_DMA
    },

    // NORMAL: 16 MiB - end of memory
    [PMM_ZONE_NORMAL] = {
        .mem_total = (size_t) -1,
        .mem_free = (size_t) -1,
        .mem_used = (size_t) -1,
        .offset = (16 * MiB),
        .bitmap = { .size = (size_t) -1, .data = NULL },
        .first_free_idx = (size_t) -1,
        .zone = PMM_ZONE_NORMAL
    }
};

/* print zone mem stats */
static inline void print_mem_stats(void) {
    log("\n");
    for (zone_e zone = 0; zone < PMM_NUM_ZONES; zone++) {
        log("zone: %lu\n", mem_zone[zone].zone);
        log("mem_total: %lx\n", mem_zone[zone].mem_total);
        log("mem_free: %lx\n", mem_zone[zone].mem_free);
        log("mem_used: %lx\n", mem_zone[zone].mem_used);
        log("offset: %lx\n", mem_zone[zone].offset);
        log("first_free_idx: %lx\n", mem_zone[zone].first_free_idx);
        log("\n");
    }
}

/* 
 * allocate physical page frames 
 * @param zone : zone to allocate in 
 * @param size : num pages to allocate 
 */
paddr_t pmm_alloc(zone_e zone, size_t size) {
    size_t idx = bitmap_find_range(&mem_zone[zone].bitmap, mem_zone[zone].first_free_idx, PMM_FREE, size);

    if (idx == (size_t) -1)
        panic("pmm_alloc could not find %lu of free pages.", size);
    
    bitmap_set_range(&mem_zone[zone].bitmap, idx, size);

    // update zone bookkeeping
    mem_zone[zone].mem_free -= size * PAGE_SIZE;
    mem_zone[zone].mem_used += size * PAGE_SIZE;
    mem_zone[zone].first_free_idx = idx + size;

    return mem_zone[zone].offset + (PAGE_SIZE * idx);
}

/* 
 * free physical page frames 
 * @param addr : PAGE_SIZE aligned physical addr to free 
 * @param size : num pages to free
 */
void pmm_free(paddr_t addr, size_t size) {
    // find zone
    zone_e zone = 0;
    for ( ; zone < PMM_NUM_ZONES; zone++) {
        if (zone + 1 == PMM_NUM_ZONES) {
            if (addr >= mem_zone[zone].offset && addr < mem_size) 
                break;
        } else {
            if (addr >= mem_zone[zone].offset && addr < mem_zone[zone + 1].offset)
                break;
        }
    }

    // verify a zone was found
    if (zone == PMM_NUM_ZONES)
        panic("pmm_free could not identify the zone for addr %lx.\n", addr);

    // free the pages
    size_t idx = (addr - mem_zone[zone].offset) / PAGE_SIZE;
    bitmap_clear_range(&mem_zone[zone].bitmap, idx, size);
    
    // update zone bookkeeping
    mem_zone[zone].mem_free += size * PAGE_SIZE;
    mem_zone[zone].mem_used -= size * PAGE_SIZE;
    if (idx < mem_zone[zone].first_free_idx)
        mem_zone[zone].first_free_idx = idx;
}

/* 
 * set region as unused 
 * @param zone : zone the bitmap is in
 * @param bit : bit within bitmap to start clearing
 * @param size : num bits to clearing 
 */
static inline void pmm_set_unused(zone_e zone, size_t bit, size_t size) {
    bitmap_clear_range(&mem_zone[zone].bitmap, bit, size);
}

void pmm_init(struct stivale2_struct* handover) {
    // get memory map information from bootloader
    struct stivale2_struct_tag_memmap* memmap = get_memmap(handover);
    if (memmap == NULL)
        panic("pmm_init could not find memory map from bootloader.");

    // verify at least 16 MiB of physical memory is present
    mem_size = get_mem_size(memmap);
    if (mem_size < (16 * MiB))
        panic("pmm_init found less than 16 MiB of memory.");

    // initialize zone structs
    for (zone_e zone = 0; zone < PMM_NUM_ZONES; zone++) {
        if (zone == PMM_NUM_ZONES - 1) 
            mem_zone[zone].mem_total = mem_size - mem_zone[zone].offset;
        else 
            mem_zone[zone].mem_total = mem_zone[zone + 1].offset - mem_zone[zone].offset;

        mem_zone[zone].mem_used = mem_zone[zone].mem_total;
        mem_zone[zone].mem_free = 0;
        mem_zone[zone].bitmap.size = mem_zone[zone].mem_total / PAGE_SIZE;

        // init bitmaps
        mem_zone[zone].bitmap.data = alloc(bitmap_size_bytes(&mem_zone[zone].bitmap), 8);
        bitmap_set_range(&mem_zone[zone].bitmap, 0, mem_zone[zone].bitmap.size);
    }

    // use bootloader handover information to set unused regions as usable
    zone_e curr_zone = PMM_ZONE_DMA;
    for (uint64_t i = 0; i < memmap->entries; i++) {
        struct stivale2_mmap_entry entry = memmap->memmap[i];
        if (entry.type == MEM_USABLE) {
            paddr_t entry_start = (paddr_t) ALIGN_UP(entry.base, 4 * KiB);                    // inclusive 
            paddr_t entry_end = (paddr_t) ALIGN_DOWN(entry.base + entry.length, 4 * KiB);     // exclusive

            // find each zone the entry crosses over
            paddr_t curr_zone_start = mem_zone[curr_zone].offset; 
            paddr_t curr_zone_end = (curr_zone + 1 == PMM_NUM_ZONES) ? mem_size : mem_zone[curr_zone + 1].offset; 

            // move curr_zone to where the entry starts
            while (curr_zone_end < entry_start) {
                curr_zone++;
                curr_zone_start = mem_zone[curr_zone].offset; 
                curr_zone_end = (curr_zone + 1 == PMM_NUM_ZONES) ? mem_size : mem_zone[curr_zone + 1].offset;
            }

            while (true) {
                size_t idx = (entry_start - curr_zone_start) / PAGE_SIZE;
                size_t size = (MIN(entry_end, curr_zone_end) - entry_start) / PAGE_SIZE;
                pmm_set_unused(curr_zone, idx, size);

                // update zone bookkeeping
                mem_zone[curr_zone].mem_used -= size * PAGE_SIZE;
                mem_zone[curr_zone].mem_free += size * PAGE_SIZE;
                if (idx < mem_zone[curr_zone].first_free_idx)
                    mem_zone[curr_zone].first_free_idx = idx;

                // entry is within current zone
                if (entry_end <= curr_zone_end)
                    break;
                
                // entry bleeds into next zone
                entry_start = MIN(curr_zone_end, entry_end);
                curr_zone++;
                curr_zone_start = (curr_zone + 1 > PMM_NUM_ZONES) ? mem_size : mem_zone[curr_zone].offset; 
                curr_zone_end = (curr_zone + 1 >= PMM_NUM_ZONES) ? mem_size : mem_zone[curr_zone + 1].offset;
            }
        }
    }
}


#include <sys/sys.h>
#include <mem/mem.h>
#include <mm/buddy.h>
#include <ds/buddy.h>

/* static area for allocating buddy system structures */
static uint8_t alloc_area[2 * MiB];
static uint8_t* alloc_ptr = alloc_area;

static inline uint8_t* alloc(size_t size) {
    uint8_t* ret = ALIGN_UP((uint64_t) alloc_ptr, 8);
    memset(ret, 0, size);
    alloc_ptr = ret + size;
    return ret;
}

/* memory usage stats */
static uint64_t kmem_used = 0;
static uint64_t kmem_free = 0;
static uint64_t kmem_total = 0;

/* buddy allocator ds */
static buddy_t buddies[BUDDY_NUM_ZONES][BUDDY_MAX_ORDER + 1];
static paddr_t buddy_offset[BUDDY_NUM_ZONES] = {
    [BUDDY_ZONE_DMA] = 0,           // DMA: 0 MiB - 16 MiB 
    [BUDDY_ZONE_NORMAL] = 16 * MiB  // NORMAL: 16 MiB - end of memory
};

/* allocation */
static size_t buddy_recursive_alloc(uint8_t zone, size_t order) {
    // verify zone and requested order are appropriate
    if (zone > BUDDY_NUM_ZONES || order > BUDDY_MAX_ORDER)
        return -1;

    if (buddies[zone][order].free_blocks) {
        // search for free block
        for (size_t idx = buddies[zone][order].first_free_idx; idx < buddies[zone][order].bitmap.size; idx++) {
            // free block found
            if (!bitmap_get(&buddies[zone][order].bitmap, idx)) {
                buddies[zone][order].first_free_idx = idx + 1;                
                buddies[zone][order].free_blocks--;

                return idx; 
            }
        }

        // free block not found, bookkeeping wrong
        error("Buddy bookkeeping error!\n");
        return -1;

    } else {
        // no block in requested order found, search for block in higher order
        size_t idx = buddy_recursive_alloc(zone, order + 1);

        // verify idx is valid
        if (idx == (size_t) -1)
            panic("buddy_recursive_alloc failed. Request to higher order failed.\n");

        // split block of higher order into 2 blocks of order
        idx <<= 1;

        // set left child as used, set right child as free
        bitmap_set(&buddies[zone][order].bitmap, idx);
        bitmap_clear(&buddies[zone][order].bitmap, BUDDY_IDX(idx));

        // update order bookkeeping 
        buddies[zone][order].free_blocks++;
        if (BUDDY_IDX(idx) < buddies[zone][order].first_free_idx)
            buddies[zone][order].first_free_idx = BUDDY_IDX(idx);

        return idx;
    }
}

paddr_t buddy_alloc(uint8_t zone, size_t size) {
    if (size <= 0) 
        return -1;

    // find smallest power of 2 block that will fit size
    size_t req_order = -1;
    for (int i = 0; i <= BUDDY_MAX_ORDER; i++) {
        if ((((size_t) BUDDY_MIN_BS) << i) > size) {
            req_order = i;
            break;
        }
    }

    // requested order could not be found
    if (req_order == (size_t) -1) 
        panic("buddy_alloc failed. Could not find requested order.\n");

    // recursively alloc block 
    size_t idx = buddy_recursive_alloc(zone, req_order);

    if (idx == (size_t) -1)
        panic("buddy_alloc failed. Recursive allocation failed.\n");

    // calculate and return physical address of free block
    return buddy_offset[zone] + (BUDDY_MIN_BS << req_order) * idx;
}

/* free */
static void buddy_recursive_free(uint8_t zone, size_t order, size_t idx) {
    // verify zone, order, and idx are valid
    if (zone >= BUDDY_NUM_ZONES || order > BUDDY_MAX_ORDER || idx >= buddies[zone][order].bitmap.size)
        return;

    // recursively combine free blocks
    if (order < BUDDY_MAX_ORDER && !bitmap_get(&buddies[zone][order].bitmap, BUDDY_IDX(idx))) {
        size_t parent_idx = (idx >> 1);

        // update parent bit and child bits
        bitmap_clear(&buddies[zone][order + 1].bitmap, parent_idx);
        bitmap_set(&buddies[zone][order].bitmap, idx);
        bitmap_set(&buddies[zone][order].bitmap, BUDDY_IDX(idx));

        // update current order bookkeeping
        buddies[zone][order].free_blocks--;
        buddies[zone][order].first_free_idx = 0;    /* fix me */

        buddy_recursive_free(zone, order + 1, parent_idx);
    } else {
        // free idx in order
        bitmap_clear(&buddies[zone][order].bitmap, idx);
        buddies[zone][order].free_blocks++;
        if (idx < buddies[zone][order].first_free_idx)
            buddies[zone][order].first_free_idx = idx;
    }
}

void buddy_free(paddr_t paddr, size_t size) {
    if (size <= 0)
        return;

    // determine zone
    uint8_t zone = -1;
    for (uint8_t i = 0; i < BUDDY_NUM_ZONES; i++) {
        if (buddy_offset[i] > paddr) {
            zone = i;
            break;
        }
    }

    if (zone == (uint8_t) -1)
        panic("buddy_free encountered attempt free from invalid physical address with invalid zone %lu\n", paddr);

    // determine order
    size_t req_order = -1;
    for (int i = 0; i <= BUDDY_MAX_ORDER; i++) {
        if ((((size_t) BUDDY_MIN_BS) << i) > size) {
            req_order = i;
            break;
        }
    }

    if (req_order == (size_t) -1) 
        panic("buddy_free encountered attempt free from invalid physical address with invalid order %lu\n", paddr);

    // determine idx
    paddr_t zone_paddr = paddr - buddy_offset[zone];
    size_t idx = zone_paddr / (BUDDY_MIN_BS << req_order);

    // recursively free
    buddy_recursive_free(zone, req_order, idx);
}

void buddy_init(uint64_t total_mem) {
    kmem_total = total_mem;

    // verify that total memory is over 16 MiB
    if (total_mem < 16 * MiB) 
        panic("More than 16 MiB of memory is needed. Only found %lu.\n", total_mem);

    // initialize buddy allocator
    for (int zone = 0; zone < BUDDY_NUM_ZONES; zone++) {
        size_t num_blocks = 0;

        if (zone == BUDDY_NUM_ZONES - 1)
            num_blocks = (total_mem - buddy_offset[BUDDY_NUM_ZONES - 1]) / BUDDY_MAX_BS;
        else
            num_blocks = (buddy_offset[zone + 1] - buddy_offset[zone]) / BUDDY_MAX_BS;    

        for (int order = BUDDY_MAX_ORDER; order >= 0; order--) {
            buddies[zone][order] = (buddy_t) {
                .free_blocks = 0, 
                .first_free_idx = -1,
                .bitmap = (bitmap_t) {
                    .size = num_blocks,
                    .data = alloc(BITS2BYTES(num_blocks))
                }
            };

            if (order == BUDDY_MAX_ORDER) {
                buddies[zone][BUDDY_MAX_ORDER].free_blocks = num_blocks;
                buddies[zone][BUDDY_MAX_ORDER].first_free_idx = 0;
                bitmap_clear_range(&buddies[zone][BUDDY_MAX_ORDER].bitmap, 0, buddies[zone][BUDDY_MAX_ORDER].bitmap.size); 
            } else {
                bitmap_set_range(&buddies[zone][order].bitmap, 0, buddies[zone][order].bitmap.size);
            }

            num_blocks <<= 1;
        }

    }

    /* TODO: use memmap to mark used regions in buddy allocator */  
}

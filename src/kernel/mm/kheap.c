#include <mm/kheap.h>
#include <mm/paging.h>
#include <mm/pmm.h>

/* 
 * lower addresses <-------------------------------> higher addresses
 * 
 *   kheap_top                                            VA_END
 * ----------------------------------------------------------
 *       |   |             |   |                            |
 *       |hdr|     free    |hdr|          used              |
 *       |   |             |   |                            |
 * ----------------------------------------------------------
 * 
 * General notes:
 * - a memblock_t header is free only if free_next and free_prev are NULL
 * - kheap_top MUST be PAGE_SIZE aligned
 * - block_head always points to memblock_t header closest to kheap_top (for kheap_expand)
 * - free_head can be any free block
 * - any block that is_free does not need to do NULL checks on free_next and free_prev
 * - freelist is a circular doubly linked list
 * - block (header) list is a normal doubly linked list
*/

/* header struct */
struct __kheap_header_t {
    struct __kheap_header_t* next;
    struct __kheap_header_t* prev;
    struct __kheap_header_t* free_next;
    struct __kheap_header_t* free_prev;
    size_t size;
};
typedef struct __kheap_header_t memblock_t;

/* utility macros */
#define is_free(hdr)            (hdr->free_next != NULL && hdr->free_prev != NULL)
#define block_start_addr(hdr)   (((vaddr_t) hdr) + sizeof(memblock_t))
#define block_end_addr(hdr)     (((vaddr_t) hdr) + sizeof(memblock_t) - 1 + hdr->size)  // NOTE: -1 necessary for VA_END

/* globals for managing heap */
vaddr_t kheap_top = 0;                          /* VA for top of heap */
vaddr_t kheap_max = 0;                          // TODO: set in paging_init
static memblock_t* free_head = NULL;            /* head of freelist headers */
static memblock_t* block_head = NULL;           /* list of ALL headers */

/* utility freelist functions */
static inline void freelist_insert(memblock_t* hdr) {
    // insert hdr into freelist
    if (free_head == NULL) {
        free_head = hdr;
        hdr->free_next = hdr;
        hdr->free_prev = hdr;
    } else {
        hdr->free_next = free_head;
        hdr->free_prev = free_head->free_prev;
        free_head->free_prev->free_next = hdr;
        free_head->free_prev = hdr;
        free_head = hdr;
    }
}

static inline void freelist_remove(memblock_t* hdr) {
    // verify freelist isn't empty
    if (free_head == NULL) {
        error("[freelist_remove] removal attempted on empty freelist.\n");
        return;
    }

    // verify hdr is actually free
    if (!is_free(hdr)) {
        error("[freelist_remove] hdr is not free.\n");
        return;
    }

    // remove hdr from freelist
    if (hdr->free_next == hdr && hdr->free_prev == hdr) {
        // one elem in freelist (hdr is free_head)
        if (hdr != free_head) {
            error("[freelist_remove] one element in freelist, but it's not hdr\n");
            return;
        }

        // freelist is empty now
        hdr->free_next = NULL;
        hdr->free_prev = NULL;
        free_head = NULL;

    } else {
        memblock_t* prev_hdr = hdr->free_prev;
        memblock_t* next_hdr = hdr->free_next;

        prev_hdr->free_next = next_hdr;
        next_hdr->free_prev = prev_hdr;
        hdr->free_next = NULL;
        hdr->free_prev = NULL;

        // hdr is free head, remove and replace free_head
        if (hdr == free_head)
            free_head = next_hdr;
    }
}

/*
 * merge_right
 * merge memory block with block/memory on the right
 * @param header : header to merge right from
 * @return new header of merged blocks
 */
static memblock_t* merge_right(memblock_t* header) {
    // verify header
    if (header == NULL) {
        error("[merge_right] header is NULL.\n");
        return NULL;
    }

    // verify header is free
    if (!is_free(header)) {
        error("[merge_right] merge_right attempt on non-free header.\n");
        return NULL;
    }

    memblock_t* right_header = header->next;

    // block on the right is free
    if (right_header != NULL && is_free(right_header)) {
        // adjust size
        header->size = (block_end_addr(right_header) - block_start_addr(header) + 1); 

        // remove right block from block (header) list 
        header->next = right_header->next;
        if (right_header->next != NULL)
            right_header->next->prev = header;

        // remove right block from free list
        freelist_remove(right_header);

    } else { 
        // merge any free mem between header and next block/end of kheap
        if (right_header == NULL) 
            header->size = VA_END - block_start_addr(header) + 1;
        else 
            header->size = ((vaddr_t) right_header) - block_start_addr(header);
    } 

    return header;
}

/*
 * merge_left
 * merge memory block with block/memory on the left
 * @param header : header to merge left from
 * @return new header of merged blocks
 */
static memblock_t* merge_left(memblock_t* header) {
    // verify header
    if (header == NULL) {
        error("[merge_right] header is NULL.\n");
        return NULL;
    }

    // verify header is free
    if (!is_free(header)) {
        error("[merge_right] merge_right attempt on non-free header.\n");
        return NULL;
    }

    memblock_t* left_header = header->prev;

    if (left_header != NULL && is_free(left_header)) {
        // adjust size
        left_header->size = (block_end_addr(header) - block_start_addr(left_header) + 1); 

        // remove block from block (header) list 
        left_header->next = header->next;
        if (header->next != NULL)
            header->next->prev = left_header;

        // remove header from free list
        freelist_remove(header);

        header = left_header;

    } else {
        // merge any free mem between header and beginning of kheap
        if (left_header == NULL) {
            // header must be block_head
            if (header != block_head) {
                error("[merge_left] no header on the left, but header is no head of block list\n");
                return NULL;
            }

            // find new size of block
            header->size = block_end_addr(header) - kheap_top - sizeof(memblock_t) + 1;

            // move header to beginning of free block
            memblock_t* old_header = header;
            header = (memblock_t*) kheap_top;
            memmove(header, old_header, sizeof(memblock_t));

        } else {
            // find new size of block
            header->size = block_end_addr(header) - block_end_addr(left_header) - sizeof(memblock_t);

            // move header to beginning of free block
            memblock_t* old_header = header;
            header = (memblock_t*) (block_end_addr(left_header) + 1);
            memmove(header, old_header, sizeof(memblock_t));
        }
    }

    return header;
}

/* 
 * merge
 * merge memory block with block/memory on the left and right
 * @param header : header to merge from
 * @return new header of merged blocks
 */
static inline memblock_t* merge(memblock_t* header) {
    header = merge_right(header);
    header = merge_left(header);
    return header;
}

/*
 * split 
 * will split the free area specified by @param hdr into 2 areas.
 * @param hdr : hdr of free area to split
 * @param num_bytes : number of bytes 
 * @return header of new free area with at least @param num_bytes
 */
static memblock_t* split(memblock_t* hdr, size_t num_bytes) {
    // verify hdr is free and non-NULL
    if (hdr == NULL || !is_free(hdr)) {
        error("[split] hdr is either NULL or not free\n");
        return NULL;
    }

    // align size request
    num_bytes = ALIGN_UP(num_bytes, 8);

    // verify free block has enough space to split
    // can only split if:
    // (hdr->size + sizeof(memblock_t)) - (num_bytes + sizeof(memblock_t)) >= sizeof(memblock_t) + KHEAP_MIN_FREE_SIZE
    if (hdr->size - num_bytes < sizeof(memblock_t) + KHEAP_MIN_FREE_SIZE)
        return hdr;

    // create new header
    hdr->size = (block_end_addr(hdr) - num_bytes - sizeof(memblock_t) + 1) - block_start_addr(hdr);
    memblock_t* new_hdr = (memblock_t*) (block_end_addr(hdr) + 1);
    new_hdr->size = num_bytes;

    // add new header to blocklist
    new_hdr->prev = hdr;
    new_hdr->next = hdr->next;
    hdr->next = new_hdr;
    if (new_hdr->next != NULL)
        new_hdr->next->prev = new_hdr;

    // add new header to freelist
    freelist_insert(new_hdr);

    return new_hdr;
}

/* 
 * kheap_expand
 * will expand the kernel heap by @param num_pages
 * TODO: map pages globally
 * @param num_pages : number of pages to expand kernel heap by
 */
static void kheap_expand(size_t num_pages) {
    // verify kheap is to be expanded
    if (num_pages == 0) {
        warning("[kheap_expand] request to expand kheap with 0 pages\n");
        return;
    }

    // map pages
    vaddr_t curr_page = kheap_top - PAGE_SIZE;
    for (size_t i = 0; i < num_pages; i++) {
        paging_map(curr_page, pmm_alloc(PMM_ZONE_NORMAL, 1), PAGE_PRESENT | PAGE_WRITABLE);
        curr_page -= PAGE_SIZE;
    }

    // calculate new kheap_top
    kheap_top -= num_pages * PAGE_SIZE;

    // make sure new kheap_top does not pass kheap_max
    if (kheap_top < kheap_max) 
        panic("[kheap_expand] kheap has expanded too much:\nkheap_top: 0x%lx\nkheap_max: 0x%lx\nnum_pages: %lu\n", kheap_top, kheap_max, num_pages);

    // create new memblock_t header
    memblock_t* header = (memblock_t*) kheap_top;

    // setup new header as block_head
    header->size = ((vaddr_t) block_head) - block_start_addr(header);
    header->next = block_head;
    header->prev = NULL;
    block_head->prev = header;
    block_head = header;

    // setup new header as head of freelist
    freelist_insert(header);

    // merge right, in case next element in block list is also free
    header = merge_right(header);
}

/* 
 * kmalloc
 * dynamically allocated memory for the kernel
 * @param size : number of bytes to allocate
 * @return pointer to newly allocated area
 */
void* kmalloc(size_t size) {
    // verify size is not 0
    if (size == 0) {
        error("[kmalloc] size of 0 was requested from kheap\n");
        return NULL;
    }

    // align up request to multiple of 8 bytes
    size = ALIGN_UP(size, 8);

    // search freelist for first free block
    memblock_t* curr = free_head;
    while (curr != NULL) {
        // allocate only if block is >= aligned size
        if (curr->size == size) {
            freelist_remove(curr);
            return (void*) block_start_addr(curr);

        } else if (curr->size > size) {
            memblock_t* new_hdr = split(curr, size);
            freelist_remove(new_hdr);
            return (void*) block_start_addr(new_hdr);
        }
        
        curr = curr->free_next;
        if (curr == free_head)
            break;
    }

    // no free block large enough found, expand heap enough to fit request
    size_t new_size = size + sizeof(memblock_t);
    size_t new_pages = (new_size / PAGE_SIZE) + (new_size % PAGE_SIZE ? 1 : 0);
    log("[kmalloc] expanding heap by %lu pages ...\n", new_pages);
    kheap_expand(new_pages);
    return kmalloc(size);
}

/* 
 * kfree
 * frees dynamically allocated memory 
 * @param size : number of bytes to allocate
 * @return pointer to newly allocated area
 */
void kfree(void* ptr) {
    // get header
    memblock_t* header = (memblock_t*) ((vaddr_t) ptr - sizeof(memblock_t));    
    
    // verify header is in use
    if (is_free(header)) {
        error("[kfree] free request called on in-use memory block\n");
        return;
    }

    // free memblock
    freelist_insert(header);

    // merge neighboring blocks
    header = merge(header);
}

/* 
 * kheap_init
 * initializes head with @param num_pages pages
 * @param num_pages : number of pages to initialize heap with
 */
void kheap_init(size_t num_pages) {
    // verify kheap is not initialized to 0 pages
    if (num_pages == 0) {
        error("[kheap_init] kheap_init was asked to initialize heap with 0 pages.\n");
        return;
    }
        
    // calculate new kheap_top
    kheap_top = VA_END - PAGE_SIZE * num_pages + 1;

    // make sure new kheap_top does not pass kheap_max
    if (kheap_top < kheap_max) 
        panic("[kheap_init] kheap was initialized with too much memory:\nkheap_top: 0x%lx\nkheap_max: 0x%lx\nnum_pages: %lu\n", kheap_top, kheap_max, num_pages);

    // map pages
    vaddr_t curr_page = VA_END - PAGE_SIZE + 1;
    for (size_t i = 0; i < num_pages; i++) {
        paging_map(curr_page, pmm_alloc(PMM_ZONE_NORMAL, 1), PAGE_PRESENT | PAGE_WRITABLE);
        curr_page -= PAGE_SIZE;
    }

    // setup memblock list and freelist
    block_head = (memblock_t*) kheap_top;
    free_head = (memblock_t*) kheap_top;
    block_head->size = (num_pages * PAGE_SIZE) - sizeof(memblock_t);
    block_head->next = NULL;
    block_head->prev = NULL;
    free_head->free_next = free_head;
    free_head->free_prev = free_head;
}

/* for debugging */
void parse_blocklist(void) {
    info("parsing kheap block (header) list ...\n");
    memblock_t* curr = block_head;
    do {
        if (curr == NULL)
            break;

        log("[parse_blocklist] header addr: 0x%lx, size: 0x%lx, free?: %u\n", curr, curr->size, is_free(curr));

        curr = curr->next;
    } while (curr != NULL);
}

void parse_freelist(void) {
    info("parsing kheap freelist ...\n");
    memblock_t* curr = free_head;
    do {
        if (curr == NULL)
            break;

        log("[parse_freelist] header addr: 0x%lx, size: 0x%lx\n", curr, curr->size);

        curr = curr->free_next;
    } while (curr != NULL && curr != free_head);
}

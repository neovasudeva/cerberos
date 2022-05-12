#include <mm/paging.h>
#include <mm/pmm.h>

void paging_intr_handler(void) {

}

/* 
 * paging_vaddr_idx 
 * returns pml index from the given level
 * @param vaddr: virtual address to find indices
 * @param level: pml to get index for
 */
static inline uint64_t paging_vaddr_idx(vaddr_t vaddr, uint8_t level) {
    return (vaddr >> (PAGE_IDX_SIZE * level + PAGE_OFFSET_SIZE)) & PAGE_IDX_MASK;
}

/* 
 * paging_cr3
 * returns the physical address of the pml4 table currently in cr3
 */
static inline pml_table_t* paging_cr3(void) {
    paddr_t cr3;
    dump_cr3(cr3);
    cr3 = ALIGN_DOWN(cr3, PAGE_SIZE);
    pml_table_t* pml4_table = (pml_table_t*) cr3;
    return pml4_table;
}

/*
 * paging_walk
 * Parses the pml tables and returns the pml entry for the given vaddr and level
 * @param pml4_table: the physical address of pml4 table to start parsing at 
 * @param vaddr: vaddr to find pml entry for
 * @param level: pml level request
 * @param create: if true, it will create non-existent pml tables while finding the pml entry with flag PAGE_PRESENT
 */
static pml_entry_t* paging_walk(pml_table_t* pml4_table, vaddr_t vaddr, uint8_t level, bool create) {
    // verify pml4 table is valid
    if (pml4_table == NULL) {
        error("[paging_maps] pml4 table is null\n");
        return NULL;
    }

    // verify vaddr is 4KiB aligned
    if (vaddr % PAGE_SIZE) {
        warning("[paging_maps] vaddr is not 4 KiB aligned, vaddr: %lx\n", vaddr); 
        vaddr = ALIGN_DOWN(vaddr, PAGE_SIZE);
    }

    // parse page table hierarchy
    pml_table_t* curr_table = pml4_table;
    for (uint8_t i = PML4; i >= level; i--) {
        uint64_t idx = paging_vaddr_idx(vaddr, i);
        pml_entry_t* pentry = &curr_table->entries[idx];

        if (i == level) 
            return pentry;

        // go to next level
        if (paging_check_flags(pentry, PAGE_PRESENT)) 
            curr_table = (pml_table_t*) paging_get_paddr(pentry);
        else if (create) {
            pml_table_t* child_table = paging_create(); 
            paging_set_paddr(pentry, (paddr_t) child_table);
            paging_set_flags(pentry, PAGE_PRESENT);
            curr_table = child_table; 
        } else {
            error("[paging_walk] A non-present entry was found at level %u for vaddr 0x%lx", level, vaddr);
            return NULL;
        }
    }

    error("[paging_walk] parsed entire page hiearchy and ended outside of loop.");
    return NULL;
}

/*
 * paging_create
 * Creates pml table on any level
 */
inline pml_table_t* paging_create(void) {
    pml_table_t* ptable = (pml_table_t*) pmm_alloc(PMM_ZONE_NORMAL, 1);
    memset(ptable, 0, sizeof(ptable));
    return ptable;
}

/*
 * paging_destroy
 * destroys @param ptable
 * @param ptable: the phyical address of pml table to delete 
 */
inline void paging_destroy(pml_table_t* ptable) {
    paddr_t paddr = (paddr_t) ptable;
    if (paddr % PAGE_SIZE) {
        warning("[paging_destroy] ptable is not 4KiB aligned.");
        paddr = ALIGN_DOWN(paddr, PAGE_SIZE);
    }

    pmm_free(paddr, 1);
}

/*
 * __paging_map
 * maps @param vaddr to @param paddr with @param flags
 * @param pml4_table: the physical address of pml4 table to map addresses at
 * @param vaddr: virtual address to map 
 * @param paddr: physical address to map to
 * @param flags: flags to create the mapping with
 */
void __paging_map(pml_table_t* pml4_table, vaddr_t vaddr, paddr_t paddr, uint64_t flags) {
    // verify pml4 table is valid
    if (pml4_table == NULL) {
        error("[paging_maps] pml4 table is null\n");
        return;
    }

    // verify vaddr is 4KiB aligned
    if (vaddr % PAGE_SIZE) {
        warning("[paging_maps] vaddr is not 4 KiB aligned, vaddr: %lx\n", vaddr); 
        vaddr = ALIGN_DOWN(vaddr, PAGE_SIZE);
    }

    // verify paddr is 4KiB aligned
    if (paddr % PAGE_SIZE) {
        warning("[paging_maps] paddr is not 4 KiB aligned, paddr: %lx\n", paddr);
        paddr = ALIGN_DOWN(paddr, PAGE_SIZE);
    }

    // get pml1 entry
    pml_entry_t* pentry = paging_walk(pml4_table, vaddr, PML1, true);

    // there is already an existing mapping, abort mapping
    if (paging_check_flags(pentry, PAGE_PRESENT)) {
        paddr_t old_paddr = paging_get_paddr(pentry);
        error("[paging_map] request to map vaddr 0x%lx to paddr 0x%lx but vaddr was already mapped to %0xlx.", vaddr, paddr, old_paddr);
        return;
    }

    // entry is empty, set addr and flags
    paging_set_flags(pentry, flags);
    paging_set_paddr(pentry, paddr);
}

/*
 * __paging_maps
 * maps @param num pages from @param vaddr to @param paddr with @param flags
 * @param pml4_table: the physical address of pml4 table to map addresses at
 * @param vaddr: virtual address to map 
 * @param paddr: physical address to map to
 * @param num: the number of pages to map
 * @param flags: flags to create the mapping with
 */
void __paging_maps(pml_table_t* pml4_table, vaddr_t vaddr, paddr_t paddr, uint64_t num, uint64_t flags) {
    for (uint64_t i = 0; i < num; i++) {
        __paging_map(pml4_table, vaddr, paddr, flags);
        vaddr += PAGE_SIZE;
        paddr += PAGE_SIZE;
    }
}

/*
 * __paging_unmap
 * unmaps @param vaddr to @param paddr 
 * @param pml4_table: the physical address of pml4 table to map addresses at
 * @param vaddr: virtual address to unmap 
 */
void __paging_unmap(pml_table_t* pml4_table, vaddr_t vaddr) {
    // verify pml4 table is valid
    if (pml4_table == NULL) {
        error("[paging_maps] pml4 table is null\n");
        return;
    }

    // verify vaddr is 4KiB aligned
    if (vaddr % PAGE_SIZE) {
        warning("[paging_maps] vaddr is not 4 KiB aligned, vaddr: %lx\n", vaddr); 
        vaddr = ALIGN_DOWN(vaddr, PAGE_SIZE);
    }

    // get pml entry
    pml_entry_t* pentry = paging_walk(pml4_table, vaddr, PML1, false);
    *pentry = 0;
}

/*
 * __paging_unmaps
 * unmaps @param num pages from @param vaddr to @param paddr 
 * @param pml4_table: the physical address of pml4 table to map addresses at
 * @param vaddr: virtual address to unmap 
 * @parama num: number pages to unmap
 */
void __paging_unmaps(pml_table_t* pml4_table, vaddr_t vaddr, uint64_t num) {
    for (uint64_t i = 0; i < num; i++) {
        __paging_unmap(pml4_table, vaddr);
        vaddr += PAGE_SIZE;
    }
}

/*
 * paging_map
 * maps page from @param vaddr to @param paddr with pml4 table currently in cr3
 * @param vaddr: virtual address to map
 * @param paddr: paddr to map
 * @parama flags: flags to make mapping with
 */
void paging_map(vaddr_t vaddr, paddr_t paddr, uint64_t flags) {
    pml_table_t* pml4_table = paging_cr3();
    __paging_map(pml4_table, vaddr, paddr, flags);
}

/*
 * paging_maps
 * maps @param num pages from @param vaddr to @param paddr with pml4 table currently in cr3
 * @param vaddr: virtual address to map
 * @param paddr: paddr to map
 * @param num: num pages to map
 * @parama flags: flags to make mapping with
 */
void paging_maps(vaddr_t vaddr, paddr_t paddr, uint64_t num, uint64_t flags) {
    for (uint64_t i = 0; i < num; i++) {
        paging_map(vaddr, paddr, flags);
        vaddr += PAGE_SIZE;
        paddr += PAGE_SIZE;
    }
}

/*
 * paging_unmap
 * unmaps page from @param vaddr with pml4 table currently in cr3
 * @param vaddr: virtual address to unmap
 */
void paging_unmap(vaddr_t vaddr) {
    pml_table_t* pml4_table = paging_cr3();
    __paging_unmap(pml4_table, vaddr);

    // invalidate page in tlb 
    invlpg(vaddr);
}

/*
 * paging_unmaps
 * unmaps @param num page from @param vaddr with pml4 table currently in cr3
 * @param vaddr: virtual address to unmap
 * @param num: number of pages to numap
 */
void paging_unmaps(vaddr_t vaddr, uint64_t num) {
    for (uint64_t i = 0; i < num; i++) {
        paging_unmap(vaddr);
        vaddr += PAGE_SIZE;
    }
}

// done
inline void paging_set_flags(pml_entry_t* pentry, uint64_t flags) {
    if (flags & PAGE_ADDR) {
        warning("[paging_set_flags] illegal set flag attempt on page entry physical address.");
        return;
    }

    *pentry |= flags;
}

// done
inline void paging_clear_flags(pml_entry_t* pentry, uint64_t flags) {
    if (flags & PAGE_ADDR) {
        warning("[paging_clear_flags] illegal clear flag attempt on page entry physical address.");
        return;
    }

    *pentry &= ~(flags);
}

// done
inline bool paging_check_flags(pml_entry_t* pentry, uint64_t flags) {
    if (flags & PAGE_ADDR) {
        warning("[paging_check_flags] illegal clear flag attempt on page entry physical address.");
        return false;
    }

    return (*pentry & flags) == flags;
}

// done
inline void paging_set_paddr(pml_entry_t* pentry, paddr_t paddr) {
    if (paddr % PAGE_SIZE) {
        warning("[paging_set_addr] paddr is not 4 KiB aligned, paddr: %lx\n", paddr);
        paddr = ALIGN_DOWN(paddr, PAGE_SIZE);
    }

    *pentry &= ~(PAGE_ADDR);
    *pentry |= (paddr & PAGE_ADDR);
}

// done
inline paddr_t paging_get_paddr(pml_entry_t* pentry) {
    return (paddr_t) (*pentry & PAGE_ADDR);
}

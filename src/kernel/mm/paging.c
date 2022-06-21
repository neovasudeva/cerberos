#include <mm/paging.h>
#include <mm/pmm.h>
#include <cpu/interrupt.h>

/* 
 * paging_init
 * initializes paging structures for a processor
 * @param handover : bootloader handover struct
 */
void paging_init(struct stivale2_struct* handover) {
    struct stivale2_struct_tag_kernel_base_address* kernel_base = (struct stivale2_struct_tag_kernel_base_address*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
    if (kernel_base == NULL)
        panic("[paging_init] stivale2 kernel base tag was not found");

    struct stivale2_struct_tag_pmrs* pmrs = (struct stivale2_struct_tag_pmrs*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_PMRS_ID);
    if (pmrs == NULL)
        panic("[paging_init] stivale2 pmrs tag was not found");

    struct stivale2_struct_tag_hhdm* hhdm_addr = (struct stivale2_struct_tag_hhdm*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_HHDM_ID);
    if (hhdm_addr == NULL)
        panic("[paging_init] stivale2 hhdm addr tag was not found");

    log("[paging_init] HHDM starting address: 0x%lx\n", hhdm_addr->addr);

    log("[paging_init] kernel physical base addr: 0x%lx, kernel virtual base address: 0x%lx\n", kernel_base->physical_base_address, kernel_base->virtual_base_address);
    log("[paging_init] pmr entries: %lu\n", pmrs->entries);
    for (uint64_t i = 0; i < pmrs->entries; i++) {
        log("[paging_init] %lu : base = 0x%lx, length = 0x%lx, permissions = 0x%lx\n", i, pmrs->pmrs[i].base, pmrs->pmrs[i].length, pmrs->pmrs[i].permissions);
    }

    struct stivale2_struct_tag_memmap* memmap = stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    if (memmap == NULL) 
        panic("[paging_init] stivale2 memmap tag not found");

    print_memmap(memmap);

    // register page fault interrupt handler
    register_intr_handler(&wrapper_page_fault_intr_handler, PAGE_FAULT_IRQ_VEC);
    
    // create pml4 table
    pml_table_t* pml4_table = paging_create();
    log("[paging_init] pml4_table: 0x%lx\n", pml4_table);

    // map pmrs
    // TODO: make these global pages
    paddr_t pbase = kernel_base->physical_base_address;
    vaddr_t vbase = kernel_base->virtual_base_address;
    for (uint64_t i = 0; i < pmrs->entries; i++) {
        struct stivale2_pmr entry = pmrs->pmrs[i];

        if (entry.length % PAGE_SIZE) 
            warning("[paging_init] pmr with base: 0x%lx has length non multiple of page size: 0x%lx.\n", entry.base, entry.length);

        vaddr_t ventry = entry.base;
        paddr_t pentry = pbase + (ventry - vbase);
        uint64_t num = entry.length / PAGE_SIZE;
        uint64_t flags = PAGE_PRESENT;

        if (entry.permissions & STIVALE2_PMR_WRITABLE) 
            flags |= PAGE_WRITABLE;

        // log("[kvm_init] ventry 0x%lx, pentry 0x%lx, num 0x%lx, flags 0x%lx\n", ventry, pentry, num, flags);
        __paging_maps(pml4_table, ventry, pentry, num, flags);
    }

    // direct map physical addr space 
    // map each region of memmap 
    // map each region of memmap with 0xffff800000000000 offset, TODO: make these global pages
    paddr_t mem_start = PAGE_SIZE;
    paddr_t mem_end = memmap->memmap[memmap->entries - 1].base + memmap->memmap[memmap->entries - 1].length;
    uint64_t mem_length = mem_end - mem_start;

    if (mem_length % PAGE_SIZE)
        warning("[paging_init] mem_length is not a multiple of page size: 0x%lx\n", mem_length);

    uint64_t num_pages = mem_length / PAGE_SIZE;

    __paging_maps(pml4_table, mem_start, mem_start, num_pages, PAGE_PRESENT | PAGE_WRITABLE);
    __paging_maps(pml4_table, P2V(mem_start), mem_start, num_pages, PAGE_PRESENT | PAGE_WRITABLE);

    // load new mappings to cr3
    load_cr3(pml4_table);
}

/*
 * page_fault_intr_handler
 * All information about error code for page fault handler is in section 4.7 in Intel docs
 * Linear address that caused fault is in CR2: https://wiki.osdev.org/CPU_Registers_x86-64#CR2
 * @param cpu_state: state of regs
 */
void page_fault_intr_handler(cpu_state_t regs) {
    // linear address that caused fault
    uint64_t cr2;
    dump_cr2(cr2);
    log("[page_fault_intr_handler] linear address that caused fault: 0x%lx\n", cr2);

    uint64_t error_code = regs.error_code;
    log("[page_fault_intr_handler] error code: 0x%lx\n", regs.error_code);
    
    // present bit
    if (error_code & PAGE_FAULT_P_MASK) log("[page_fault_intr_handler] fault caused by page-level protection violation.\n");
    else log("[page_fault_intr_handler] fault caused by non-present page.\n");

    // read/write bit
    if (error_code & PAGE_FAULT_WR_MASK) log("[page_fault_intr_handler] access was a write.\n");
    else log("[page_fault_intr_handler] access was read.\n");

    // user/supervisor
    if (error_code & PAGE_FAULT_US_MASK) log("[page_fault_intr_handler] user-mode access caused fault.\n");
    else log("[page_fault_intr_handler] supervisor-mode access caused fault\n");

    // reserved bit
    if (error_code & PAGE_FAULT_RSVD_MASK) log("[page_fault_intr_handler] fault was caused by a reserved bit set to 1 in some paging-structure entry.\n");
    else log("[page_fault_intr_handler] fault was not caused by reserved bit violation\n");

    // instruction fetch
    if (error_code & PAGE_FAULT_IF_MASK) log("[page_fault_intr_handler] fault was caused by instruction fetch.\n");
    else log("[page_fault_intr_handler] fault was not caused by instruction fetch.\n");

    // protection keys
    if (error_code & PAGE_FAULT_PK_MASK) log("[page_fault_intr_handler] fault was caused by protection key violation.\n");
    else log("[page_fault_intr_handler] fault was not caused by protection key violation.\n");

    // sgx
    if (error_code & PAGE_FAULT_SGX_MASK) log("[page_fault_intr_handler] fault resulted from violation of SGX-specific access-control requirements.\n");
    else log("[page_fault_intr_handler] fault was not SGX-related.\n");

    // halt 
    panic("[page_fault_intr_handler] A page fault occurred! Have fun debugging buddy.");
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
        error("[paging_walk] pml4 table is null\n");
        return NULL;
    }

    // verify vaddr is 4KiB aligned
    if (vaddr % PAGE_SIZE) {
        warning("[paging_walk] vaddr is not 4 KiB aligned, vaddr: %lx\n", vaddr); 
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
            error("[paging_walk] A non-present entry was found at level %u for vaddr 0x%lx\n", level, vaddr);
            return NULL;
        }
    }

    error("[paging_walk] parsed entire page hiearchy and ended outside of loop.\n");
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
        warning("[paging_destroy] ptable is not 4KiB aligned.\n");
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
        error("[__paging_map] pml4 table is null\n");
        return;
    }

    // verify vaddr is 4KiB aligned
    if (vaddr % PAGE_SIZE) {
        warning("[__paging_map] vaddr is not 4 KiB aligned, vaddr: %lx\n", vaddr); 
        vaddr = ALIGN_DOWN(vaddr, PAGE_SIZE);
    }

    // verify paddr is 4KiB aligned
    if (paddr % PAGE_SIZE) {
        warning("[__paging_map] paddr is not 4 KiB aligned, paddr: %lx\n", paddr);
        paddr = ALIGN_DOWN(paddr, PAGE_SIZE);
    }

    // get pml1 entry
    pml_entry_t* pentry = paging_walk(pml4_table, vaddr, PML1, true);

    // there is already an existing mapping, abort mapping
    if (paging_check_flags(pentry, PAGE_PRESENT)) {
        paddr_t old_paddr = paging_get_paddr(pentry);
        error("[__paging_map] request to map vaddr 0x%lx to paddr 0x%lx but vaddr was already mapped to %0xlx.\n", vaddr, paddr, old_paddr);
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
        error("[__paging_unmap] pml4 table is null\n");
        return;
    }

    // verify vaddr is 4KiB aligned
    if (vaddr % PAGE_SIZE) {
        warning("[__paging_unmap] vaddr is not 4 KiB aligned, vaddr: %lx\n", vaddr); 
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

/*
 * paging_set_flags
 * sets @param flags in @param pentry
 * @param pentry: pml entry to edit flags
 * @param flags: flags to set
 */
inline void paging_set_flags(pml_entry_t* pentry, uint64_t flags) {
    if (flags & PAGE_ADDR) {
        warning("[paging_set_flags] illegal set flag attempt on page entry physical address.\n");
        return;
    }

    *pentry |= flags;
}

/*
 * paging_clear_flags
 * clears @param flags in @param pentry
 * @param pentry: pml entry to edit flags
 * @param flags: flags to clear
 */
inline void paging_clear_flags(pml_entry_t* pentry, uint64_t flags) {
    if (flags & PAGE_ADDR) {
        warning("[paging_clear_flags] illegal clear flag attempt on page entry physical address.\n");
        return;
    }

    *pentry &= ~(flags);
}

/*
 * paging_check_flags
 * checks if @param flags are set in @param pentry
 * @param pentry: pml entry to check flags for
 * @param flags: flags to check
 */
inline bool paging_check_flags(pml_entry_t* pentry, uint64_t flags) {
    if (flags & PAGE_ADDR) {
        warning("[paging_check_flags] illegal clear flag attempt on page entry physical address.\n");
        return false;
    }

    return (*pentry & flags) == flags;
}

/*
 * paging_set_paddr
 * sets @param paddr in @param pentry
 * @param pentry: pml entry to set physical address
 * @param paddr: physical address 
 */
inline void paging_set_paddr(pml_entry_t* pentry, paddr_t paddr) {
    if (paddr % PAGE_SIZE) {
        warning("[paging_set_addr] paddr is not 4 KiB aligned, paddr: %lx\n", paddr);
        paddr = ALIGN_DOWN(paddr, PAGE_SIZE);
    }

    *pentry &= ~(PAGE_ADDR);
    *pentry |= (paddr & PAGE_ADDR);
}

/*
 * paging_get_paddr
 * gets physical address in @param pentry
 * @param pentry: pml entry to get physical address
 */
inline paddr_t paging_get_paddr(pml_entry_t* pentry) {
    return (paddr_t) (*pentry & PAGE_ADDR);
}

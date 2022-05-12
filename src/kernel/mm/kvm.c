#include <mm/kvm.h>
#include <mm/pmm.h>
#include <mm/paging.h>
#include <mm/memmap.h>

#include <log.h>

void kvm_init(struct stivale2_struct* handover) {
    struct stivale2_struct_tag_kernel_base_address* kernel_base = (struct stivale2_struct_tag_kernel_base_address*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
    if (kernel_base == NULL)
        panic("stivale2 kernel base tag was not found");

    struct stivale2_struct_tag_pmrs* pmrs = (struct stivale2_struct_tag_pmrs*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_PMRS_ID);
    if (pmrs == NULL)
        panic("stivale2 pmrs tag was not found");

    struct stivale2_struct_tag_hhdm* hhdm_addr = (struct stivale2_struct_tag_hhdm*) stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_HHDM_ID);
    if (hhdm_addr == NULL)
        panic("stivale2 hhdm addr tag was not found");

    log("HHDM starting address: %lx\n", hhdm_addr->addr);

    log("kernel physical base addr: 0x%lx\nkernel virtual base address: 0x%lx\n", kernel_base->physical_base_address, kernel_base->virtual_base_address);
    log("pmr entries: %lu\n", pmrs->entries);
    for (uint64_t i = 0; i < pmrs->entries; i++) {
        log("%lu : base = 0x%lx, length = 0x%lx, permissions = 0x%lx\n", i, pmrs->pmrs[i].base, pmrs->pmrs[i].length, pmrs->pmrs[i].permissions);
    }

    struct stivale2_struct_tag_memmap* memmap = stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    if (memmap == NULL) 
        panic("stivale2 memmap tag not found");

    print_memmap(memmap);
    
    // create pml4 table
    pml_table_t* pml4_table = paging_create();
    log("pml4_table: 0x%lx\n", pml4_table);

    // map pmrs
    // TODO: make these global pages
    paddr_t pbase = kernel_base->physical_base_address;
    vaddr_t vbase = kernel_base->virtual_base_address;
    for (uint64_t i = 0; i < pmrs->entries; i++) {
        struct stivale2_pmr entry = pmrs->pmrs[i];

        if (entry.length % PAGE_SIZE) 
            warning("[kvm_init] pmr with base: 0x%lx has length non multiple of page size: 0x%lx.\n", entry.base, entry.length);

        vaddr_t ventry = entry.base;
        paddr_t pentry = pbase + (ventry - vbase);
        uint64_t num = entry.length / PAGE_SIZE;
        uint64_t flags = PAGE_PRESENT;

        if (entry.permissions & STIVALE2_PMR_WRITABLE) 
            flags |= PAGE_WRITABLE;

        log("[kvm_init] ventry 0x%lx, pentry 0x%lx, num 0x%lx, flags 0x%lx\n", ventry, pentry, num, flags);
        __paging_maps(pml4_table, ventry, pentry, num, flags);
    }

    // direct map physical addr space 
    // map each region of memmap 
    // map each region of memmap with 0xffff800000000000 offset, TODO: make these global pages
    paddr_t mem_start = PAGE_SIZE;
    paddr_t mem_end = memmap->memmap[memmap->entries - 1].base + memmap->memmap[memmap->entries - 1].length;
    uint64_t mem_length = mem_end - mem_start;

    if (mem_length % PAGE_SIZE)
        warning("[kvm_init] mem_length is not a multiple of page size: 0x%lx\n", mem_length);

    uint64_t num_pages = mem_length / PAGE_SIZE;

    __paging_maps(pml4_table, mem_start, mem_start, num_pages, PAGE_PRESENT | PAGE_WRITABLE);
    __paging_maps(pml4_table, P2V(mem_start), mem_start, num_pages, PAGE_PRESENT | PAGE_WRITABLE);

    // map kernel heap (map singular page )
    // TODO: make these global pages
    __paging_map(pml4_table, KHEAP_INIT_VADDR, pmm_alloc(PMM_ZONE_NORMAL, 1), PAGE_PRESENT | PAGE_WRITABLE);

    log("neo!\n");

    // TODO: map (map singular page up until 16 MiB) kernel eternal heap?
    // TODO: make these global pages

    // load new mappings to cr3
    load_cr3(pml4_table);
}
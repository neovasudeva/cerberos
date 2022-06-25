#pragma once

#include <sys/sys.h>
#include <mem/mem.h>
#include <mm/mem.h>
#include <boot/stivale2.h>
#include <intr/interrupt.h>

#define PAGE_FAULT_IRQ_VEC      0xE
#define PAGE_FAULT_P_MASK       (1 << 0)
#define PAGE_FAULT_WR_MASK      (1 << 1)
#define PAGE_FAULT_US_MASK      (1 << 2)
#define PAGE_FAULT_RSVD_MASK    (1 << 3)
#define PAGE_FAULT_IF_MASK      (1 << 4)
#define PAGE_FAULT_PK_MASK      (1 << 5)
#define PAGE_FAULT_SGX_MASK     (1 << 15)

#define PAGE_OFFSET_SIZE    12
#define PAGE_IDX_SIZE       9
#define PAGE_IDX_MASK       ((uint64_t) 0x1ff)

/* 
 * 4-level paging
 * cr3 -> pml4 -> pml3 (pdpt) -> pml2 (pdt) -> pml1 (pt) -> page frame
 * refer to section 4.5 of Intel manual
 */

typedef enum {
    PAGE_PRESENT =          0x0000000000000001,
    PAGE_WRITABLE =         0x0000000000000002, 
    PAGE_USER =             0x0000000000000004, 
    PAGE_WRITE_THROUGH =    0x0000000000000008, 
    PAGE_CACHE_DISABLE =    0x0000000000000010, 
    PAGE_ACCESSED =         0x0000000000000020,
    PAGE_PSIZE =            0x0000000000000080, // not applicable to pml4 table
    PAGE_ADDR =             0x000ffffffffff000,
    PAGE_EXECUTE_DISABLE =  0x8000000000000000
} pmask_e;

/* levels */
#define PML4    3
#define PML3    2
#define PML2    1
#define PML1    0

/* entries */
typedef uint64_t pml_entry_t;

/* tables */
typedef struct {
    pml_entry_t entries[512];
} pml_table_t;

void paging_init(struct stivale2_struct* handover);

/* page fault handler */
void page_fault_intr_handler(cpu_state_t regs);
extern void wrapper_page_fault_intr_handler(void);

/* paging api */
pml_table_t* paging_create(void);
void paging_destroy(pml_table_t* ptable);

void __paging_map(pml_table_t* pml4_table, vaddr_t vaddr, paddr_t paddr, uint64_t flags);
void __paging_maps(pml_table_t* pml4_table, vaddr_t vaddr, paddr_t paddr, uint64_t num, uint64_t flags);
void __paging_unmap(pml_table_t* pml4_table, vaddr_t vaddr);
void __paging_unmaps(pml_table_t* pml4_table, vaddr_t vaddr, uint64_t num);

void paging_map(vaddr_t vaddr, paddr_t paddr, uint64_t flags);
void paging_maps(vaddr_t vaddr, paddr_t paddr, uint64_t num, uint64_t flags);
void paging_unmap(vaddr_t vaddr);
void paging_unmaps(vaddr_t vaddr, uint64_t num);

void paging_set_flags(pml_entry_t* pentry, uint64_t flags);
void paging_clear_flags(pml_entry_t* pentry, uint64_t flags);
bool paging_check_flags(pml_entry_t* pentry, uint64_t flags);
void paging_set_paddr(pml_entry_t* pentry, paddr_t paddr);
paddr_t paging_get_paddr(pml_entry_t* pentry);

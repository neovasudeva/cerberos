#pragma once

#include <sys/sys.h>
#include <boot/stivale2.h>
#include <mm/paging.h>
#include <mm/mem.h>

#define KHEAP_INIT_VADDR    (VA_END - PAGE_SIZE + 1)

void kvm_init(struct stivale2_struct* handover);
// kmalloc
// kfree
// krealloc
// kcalloc
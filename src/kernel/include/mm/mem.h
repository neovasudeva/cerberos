#pragma once

#include <sys/sys.h>

typedef uintptr_t paddr_t;
typedef uintptr_t vaddr_t;

#define PAGE_SIZE   (4 * KiB)

#define VA_END      (0xffffffffffffffff)
#define VA_HHDM     (0xffff800000000000)
#define VA_KBASE    (0xffffffff80000000)
#define V2P(vaddr)  (vaddr - VA_HHDM)
#define P2V(paddr)  (paddr + VA_HHDM)
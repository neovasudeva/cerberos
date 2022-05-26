#pragma once

#include <sys/sys.h>
#include <mm/mem.h>
#include <stivale/stivale2.h>

#define KHEAP_INIT_PAGES    1
#define KHEAP_MIN_FREE_SIZE 1

/* kernel heap api */
void* kmalloc(size_t size);
void kfree(void* ptr);

void kheap_init(size_t num_pages);

/* debugging */
void parse_blocklist(void);
void parse_freelist(void);



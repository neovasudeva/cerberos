#include <boot/stivale2.h>
#include <sys/sys.h>

/* 
 * for more information:
 * https://wiki.osdev.org/Stivale_Bare_Bones
 * https://github.com/stivale/stivale/blob/master/STIVALE2.md
 */
 
static uint8_t stack[16 * KiB];

/* 
 * kernel to bootloader (header) tags
 */
 
// NO TAGS

/*
 * bootloader to kernel (struct) tags
 */

static struct stivale2_struct_tag_kernel_base_address kernel_base_addr_struct_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID,
        .next = 0
    }
};

static struct stivale2_struct_tag_pmrs pmr_struct_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_PMRS_ID,
        .next = (uint64_t) &kernel_base_addr_struct_tag
    }
};

static struct stivale2_struct_tag_hhdm hhdm_struct_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_HHDM_ID,
        .next = (uint64_t) &pmr_struct_tag
    }
};

static struct stivale2_struct_tag_rsdp rsdp_struct_tag = {
    .tag = {
        .identifier = STIVALE2_STRUCT_TAG_RSDP_ID,
        .next = (uint64_t) &hhdm_struct_tag
    }
};

/*
 * stivale header tag
 */
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uint64_t) &rsdp_struct_tag
};
 
/* 
 * function to parse bootloader-to-kernel tags
 */ 
void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) {
            return NULL;
        }
 
        if (current_tag->identifier == id) {
            return current_tag;
        }
 
        current_tag = (void *)current_tag->next;
    }
}
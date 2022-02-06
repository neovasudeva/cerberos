#include <boot/stivale2.h>

/* 
 * for more information:
 * https://wiki.osdev.org/Stivale_Bare_Bones
 * https://github.com/stivale/stivale/blob/master/STIVALE2.md
 */
 
static uint8_t stack[16384];

/* 
 * kernel to bootloader tags
 */
 
// NO TAGS

/*
 * stivale header tag
 */
 
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = 0
};
 
/*
 * bootloader to kernel tags
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
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGE_USED   0
#define PAGE_FREE   1

#define get_byte_index(bit) (bit / 8)
#define get_bit_index(bit) (bit % 8)

typedef struct {
    size_t size;    /* size in bits */
    uint8_t* data;
} bitmap_t;

static inline void bitmap_set(bitmap_t* bitmap, size_t bit) {
    const size_t byte_index = get_byte_index(bit);
    const size_t bit_index = get_bit_index(bit);

    bitmap->data[byte_index] |= (1 << bit_index);
}

static inline void bitmap_clear(bitmap_t* bitmap, size_t bit) {
    const size_t byte_index = get_byte_index(bit);
    const size_t bit_index = get_bit_index(bit);

    bitmap->data[byte_index] &= ~(1 << bit_index);
}

static inline bool bitmap_get(bitmap_t* bitmap, size_t bit) {
    const size_t byte_index = get_byte_index(bit);
    const size_t bit_index = get_bit_index(bit);

    return bitmap->data[byte_index] & (1 << bit_index);  
}

static inline void bitmap_set_range(bitmap_t* bitmap, size_t bit, size_t num_bits, bool value) {
    for (size_t i = 0; i < num_bits; i++) {
        if (value)
            bitmap_set(bitmap, bit + i);
        else   
            bitmap_clear(bitmap, bit + i);
    }
}

static inline size_t bitmap_size_bytes(bitmap_t* bitmap) {
    return (bitmap->size / 8) + (bitmap->size % 8 ? 1 : 0);
}

static inline size_t bitmap_size_pages(bitmap_t* bitmap, uint64_t page_size) {
    size_t size_bytes = bitmap_size_bytes(bitmap);
    return (size_bytes / page_size) + (size_bytes % page_size ? 1 : 0);
}

bitmap_t bitmap_create(size_t size, uint8_t* data); 
void bitmap_fill(bitmap_t* bitmap, bool value);

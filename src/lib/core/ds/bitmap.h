#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define get_byte_index(bit) (bit / 8)
#define get_bit_index(bit) (bit % 8)

typedef struct {
    size_t size;
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

bitmap_t bitmap_create(size_t size, uint8_t* data); 
void bitmap_fill(bitmap_t* bitmap, bool value);

#include "bitmap.h"

bitmap_t bitmap_create(size_t size, uint8_t* data) {
    return (bitmap_t) { .size = size, .data = data };
}

void bitmap_fill(bitmap_t* bitmap, bool value) {
    bitmap_set_range(bitmap, 0, bitmap->size, value);
}

#pragma once

#include <ds/bitmap.h>

typedef struct {
    size_t free_blocks;
    size_t first_free_idx;
    bitmap_t bitmap;
} buddy_t;

#define BUDDY_IDX(idx) (idx ^ 0x01)

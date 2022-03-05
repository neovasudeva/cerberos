#pragma once

#define UNUSED(expr)                ((void) expr)
#define ALIGN_DOWN(addr, align)     (void*) ((addr) & ~(align - 1))
#define ALIGN_UP(addr, align)       (void*) ((addr + align - 1) & ~(align - 1))
#define BITS2BYTES(bits)            ((bits / 8) + (bits % 8 ? 1 : 0))

#define MIN(a, b)   ((a < b) ? (a) : (b))
#define MAX(a, b)   ((a > b) ? (a) : (b))

typedef uint64_t paddr_t;
typedef uint64_t vaddr_t;

#define KiB  (1 << 10)
#define MiB  (1 << 20)
#define GiB  (1 << 30)
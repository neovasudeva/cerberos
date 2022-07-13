#pragma once

/* general utilities */
#define UNUSED(expr)                ((void) expr)
#define CAST(data, type)            ((type) data)

/* string utilities */
#define CONCAT(a, b)                CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b)          a ## b

/* math/address calculation utilities */
#define MIN(a, b)                   ((a < b) ? (a) : (b))
#define MAX(a, b)                   ((a > b) ? (a) : (b))
#define ALIGN_DOWN(addr, align)     ((uintptr_t) ((addr) & ~(align - 1)))
#define ALIGN_UP(addr, align)       ((uintptr_t) ((addr + align - 1) & ~(align - 1)))
#define BITS2BYTES(bits)            ((bits / 8) + (bits % 8 ? 1 : 0))

/* size utilities */
#define KiB  (1 << 10)
#define MiB  (1 << 20)
#define GiB  (1 << 30)
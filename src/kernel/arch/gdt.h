#pragma once

#include <stdint.h>

/* 
 * for more information:
 * https://wiki.osdev.org/Global_Descriptor_Table
 */

#define GDT_ENTRIES 5

/* access bits */
#define GDT_ACCESS_PRESENT  (0b10000000)
#define GDT_ACCESS_PL3      (0b01100000)  
#define GDT_ACCESS_SEG_SYS  (0b00010000)
#define GDT_ACCESS_EXEC     (0b00001000)
#define GDT_ACCESS_RW       (0b00000010)

/* flags bits */
#define GDT_FLAGS_LONG_MODE (0b0010)

typedef struct {
    uint32_t reserve1;
    uint64_t rsp[3];
    uint64_t reserve2;
    uint64_t ist[7];
    uint64_t reserve3;
    uint16_t reserve4;
    uint16_t iopb_offset;
} __attribute__((packed)) tss_t;

/* 64-bit tss descriptor */
/* 
 * for some reason not on osdev? 
 * refer to Figure 7-4 in Intel 64 and IA32 Software Developer's Manual Volume 3a 
 */
typedef struct {
    uint16_t limit_15_0;
    uint16_t base_15_0;
    uint8_t base_23_16;
    uint8_t type : 4;
    uint8_t zeroes1 : 1;
    uint8_t dpl : 2;
    uint8_t present : 1;
    uint8_t limit_19_16 : 4;
    uint8_t avl : 1;
    uint8_t zeroes2 : 2;
    uint8_t granularity : 1;
    uint8_t base_31_24;
    uint32_t base_63_32;
    uint8_t reserve1;
    uint8_t zeroes3 : 5;
    uint64_t reserve2 : 19;
} __attribute__((packed)) tss_descriptor_t;

/* will lgdt with this struct */
typedef struct {
    uint16_t size;
    uint64_t address;
} __attribute__((packed)) gdt_info_t;

/* segment descriptor --> entries in the gdt */
/* Note that in 64-bit mode, limit and base are ignored */
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags : 4;
    uint8_t base_high;
} __attribute__((packed)) seg_descriptor_t;

/* gdt */
typedef struct {
    seg_descriptor_t gdt_entries[GDT_ENTRIES];
    tss_descriptor_t tss_descrip;
} __attribute__((packed)) gdt_t;

seg_descriptor_t gdt_create_entry(uint32_t limit, uint32_t base, uint8_t access, uint8_t flags);
tss_descriptor_t gdt_create_tss_entry(tss_t* tss_ptr);
void gdt_init(void);
extern void gdt_load();

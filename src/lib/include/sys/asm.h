#pragma once

/* interrupts */
#define hlt()   asm volatile ("hlt")
#define cli()   asm volatile ("cli")
#define sti()   asm volatile ("sti")

/* x86 paging */
#define load_cr3(val)   asm volatile("mov %0, %%cr3" : : "r" ((uint64_t) val) : )
#define dump_cr3(val)   asm volatile("mov %%cr3, %0" : "=r" (val) : : )
#define invlpg(vaddr)   asm volatile("invlpg (%0)" : : "r" (vaddr) : "memory")
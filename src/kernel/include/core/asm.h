#pragma once

/* assembly instructions */
#define hlt()   asm volatile ("hlt")
#define cli()   asm volatile ("cli")
#define sti()   asm volatile ("sti")

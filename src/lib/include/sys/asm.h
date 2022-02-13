#pragma once

#define hlt()   asm volatile ("hlt")
#define cli()   asm volatile ("cli")
#define sti()   asm volatile ("sti")

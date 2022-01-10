/*
 * for more information:
 * https://wiki.osdev.org/Stivale_Bare_Bones
 */

ENTRY(_start)
 
SECTIONS
{
    . = 0xffffffff80000000;

    .stivale2hdr : {
        KEEP(*(.stivale2hdr))
    }
 
    .text : {
        *(.text .text.*)
    } 
 
    .rodata : {
        *(.rodata .rodata.*)
    } 
 
    .data : {
        *(.data .data.*)
    } 
 
    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } 
}
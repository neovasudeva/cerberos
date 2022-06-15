; for more information
; https://littleosbook.github.io/#interrupts-and-input
;
; also a note about interrupt flag during interrupt handling
; https://forum.osdev.org/viewtopic.php?f=1&t=20572

section .text
global idt_load
extern idt_info
idt_load:
    lidt [idt_info]
    ;sti ; <-- enable interrupts
    ret

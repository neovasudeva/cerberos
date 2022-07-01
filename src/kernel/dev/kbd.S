%include "intr/asm_util.S"

;%define KBD_INTR_VEC 0x21
%define KBD_IRQ         1
%define KBD_INTR_VEC    (KBD_IRQ + INTR_DEV_OFFSET)

global wrapper_kbd_intr_handler
extern kbd_intr_handler

wrapper_kbd_intr_handler:
    push qword 0              ; <-- a dummy error code of 0
    push qword KBD_INTR_VEC   ; <-- interrupt vector
    push_all
    call kbd_intr_handler
    pop_all
    
    ; pop error code and interrupt vector
    add rsp, 16

    iretq


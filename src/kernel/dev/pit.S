%include "intr/asm_util.S"

%define PIT_IRQ         0
%define PIT_INTR_VEC    (PIT_IRQ + INTR_DEV_OFFSET)

global wrapper_pit_intr_handler
extern pit_intr_handler

wrapper_pit_intr_handler:
    push qword 0              ; <-- a dummy error code of 0
    push qword PIT_INTR_VEC   ; <-- interrupt vector
    push_all
    call pit_intr_handler
    pop_all
    
    ; pop error code and interrupt vector
    add rsp, 16

    iretq
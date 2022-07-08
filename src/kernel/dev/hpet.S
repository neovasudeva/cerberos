%include "intr/asm_util.S"

%define HPET_IRQ         0
%define HPET_INTR_VEC    (HPET_IRQ + INTR_DEV_OFFSET)

global wrapper_hpet_intr_handler
extern hpet_intr_handler

wrapper_hpet_intr_handler:
    push qword 0              ; <-- a dummy error code of 0
    push qword HPET_INTR_VEC   ; <-- interrupt vector
    push_all
    call hpet_intr_handler
    pop_all
    
    ; pop error code and interrupt vector
    add rsp, 16

    iretq
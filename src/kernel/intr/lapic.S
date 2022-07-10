%include "intr/asm_util.S"

%define LAPIC_TIMER_IRQ         10
%define LAPIC_TIMER_INTR_VEC    (LAPIC_TIMER_IRQ + INTR_DEV_OFFSET)

global wrapper_lapic_timer_intr_handler
extern lapic_timer_intr_handler

wrapper_lapic_timer_intr_handler:
    push qword 0                        ; <-- a dummy error code of 0
    push qword LAPIC_TIMER_INTR_VEC     ; <-- interrupt vector
    push_all
    call lapic_timer_intr_handler
    pop_all
    
    ; pop error code and interrupt vector
    add rsp, 16

    iretq
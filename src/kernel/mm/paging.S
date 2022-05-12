%include "asm_util.S"

%define PAGE_FAULT_INTR_VEC 0xE

global wrapper_page_fault_intr_handler
extern page_fault_intr_handler

wrapper_page_fault_intr_handler:
    ; error code already pushed on stack
    push qword PAGE_FAULT_INTR_VEC  ; <-- interrupt vector
    push_all
    call page_fault_intr_handler
    pop_all
    
    ; pop error code and interrupt vector
    add rsp, 16
    iretq
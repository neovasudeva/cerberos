%include "src/kernel/arch/interrupt_util.S"

%macro isr_err 1
intr_%1:
    ; err code already pushed
    push qword %1   ; <-- interrupt vector
    jmp common_intr_handler
%endmacro    

%macro isr_no_err 1
intr_%1:
    push qword 0    ; <-- a dummy error code of 0
    push qword %1   ; <-- interrupt vector
    jmp common_intr_handler
%endmacro

%macro isr_addr 1
    dq intr_%1
%endmacro

section .text
extern generic_intr_handler

; "do i need to sti and cli in interrupt handler?"
; https://forum.osdev.org/viewtopic.php?f=1&t=20572
common_intr_handler:
    push_all
    call generic_intr_handler
    pop_all

    ; pop error code and interrupt vector
    add rsp, 16

    iretq

; exception table
; more info at https://wiki.osdev.org/Exceptions
isr_no_err 0
isr_no_err 1
isr_no_err 2
isr_no_err 3
isr_no_err 4
isr_no_err 5
isr_no_err 6
isr_no_err 7
isr_err    8
isr_no_err 9
isr_err    10
isr_err    11
isr_err    12
isr_err    13
isr_err    14
isr_no_err 15
isr_no_err 16
isr_err    17
isr_no_err 18
isr_no_err 19
isr_no_err 20
isr_no_err 21
isr_no_err 22
isr_no_err 23
isr_no_err 24
isr_no_err 25
isr_no_err 26
isr_no_err 27
isr_no_err 28
isr_no_err 29
isr_err    30
isr_no_err 31

%assign i 32
%rep 224
    isr_no_err i
%assign i i+1
%endrep

; isr_addr_table contains data of each isr's address
section .data
global isr_addr_table
isr_addr_table:
%assign i 0
%rep 256
    isr_addr i
%assign i i+1
%endrep
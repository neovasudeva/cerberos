; for more information
; https://littleosbook.github.io/#interrupts-and-input
;
; also a note about interrupt flag during interrupt handling
; https://forum.osdev.org/viewtopic.php?f=1&t=20572

global idt_load
extern idt_info
idt_load:
    lidt [idt_info]
    sti ; <-- enable interrupts
    ret

%macro push_all 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro pop_all 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro isr_err 1
int_%1:
    ; err code already pushed
    push qword %1   ; <-- interrupt vector
    jmp common_int_handler
%endmacro    

%macro isr_no_err 1
int_%1:
    push qword 0    ; <-- a dummy error code of 0
    push qword %1   ; <-- interrupt vector
    jmp common_int_handler
%endmacro

%macro isr_addr 1
    dq int_%1
%endmacro

extern interrupt_handler

common_int_handler:
    push_all
    call interrupt_handler
    pop_all

    ; pop error code and interrupt vector
    add rsp, 16

    iret

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
global isr_addr_table
isr_addr_table:
%assign i 0
%rep 256
    isr_addr i
%assign i i+1
%endrep
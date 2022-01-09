global gdt_load
extern gdt_info

; because nasm doesn't allow segment overriding
; we have to change the code segment with this hack https://web.itu.edu.tr/kesgin/mul06/intel/instr/ret_retf.html
gdt_load:
    lgdt [gdt_info]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, qword hack
    push qword 0x8
    push rax
    retf

hack:
    ret


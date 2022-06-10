#include <stdint.h>
#include <sys/sys.h>
#include <log.h>

#include <boot/stivale2.h>
#include <dev/text.h>
#include <dev/com.h>
#include <dev/pic.h>
#include <dev/kbd.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <mm/pmm.h>
#include <mm/kvm.h>
#include <acpi/acpi.h>

/*
 * entry point for kernel 
 */
void _start(struct stivale2_struct* handover) {
    com_init();
    gdt_init();
    idt_init();
    pic_disable();
    pmm_init(handover);
    kvm_init(handover);
    acpi_init(handover);

    for (;;) {
        hlt();
    }
}
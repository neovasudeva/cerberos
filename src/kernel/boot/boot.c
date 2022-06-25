#include <stdint.h>
#include <sys/sys.h>
#include <log.h>

#include <boot/stivale2.h>
#include <dev/text.h>
#include <dev/com.h>
#include <dev/kbd.h>
#include <intr/pic.h>
#include <intr/idt.h>
#include <intr/apic.h>
#include <mm/gdt.h>
#include <mm/pmm.h>
#include <mm/paging.h>
#include <mm/kheap.h>
#include <cpu/smp.h>
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
    paging_init(handover);

    // TODO: make kernel heap global pages
    kheap_init(handover, KHEAP_INIT_PAGES);
    // TODO: map (map singular page up until 16 MiB) kernel eternal heap?
    // kheap_eternal_init();

    acpi_init(handover);
    apic_init(handover);
    smp_init(handover);

    /* devices */
    kbd_init();

    for (;;) {
        hlt();
    }
}
#include <dev/pit.h>
#include <intr/apic.h>
#include <log.h>

void pit_init(void) {
    log("[pit_init] initializing PIT...\n");

    // setup in ioapic and IDT
    register_intr_handler(&wrapper_pit_intr_handler, PIT_INTR_VEC);
    ioapic_quickset_irq(find_ioapic(PIT_IRQ), find_redtbl_entry(PIT_IRQ), bsp_lapic_id(), PIT_INTR_VEC);

    // command and frequency divider
    uint16_t freq = (uint16_t) (PIT_FREQ / PIT_DIV_1KHZ);
    pit_cmd_t cmd = {
        .bcd_bin_mode   = PIT_BIN_MODE,
        .operating_mode = PIT_MODE2,
        .access_mode    = PIT_LOW_HIGH,
        .channel        = PIT_CHANNEL0
    };


    // write command to CMD port and feed frequency divider to channel 0
    outb(cmd.raw, PIT_CMD_PORT);
    outb((freq & 0xff), PIT_CHANNEL0_PORT);
    outb(((freq >> 8) & 0xff), PIT_CHANNEL0_PORT);
}

void pit_intr_handler(void) {
    //log("[pit_intr_handler] hello world!\n");
    lapic_eoi(PIT_INTR_VEC);
}
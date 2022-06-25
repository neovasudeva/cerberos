#pragma once

#include <sys/sys.h>
#include <intr/lapic.h>
#include <intr/ioapic.h>
#include <boot/stivale2.h>

void apic_init(struct stivale2_struct* handover);
uint32_t bsp_lapic_id(void);
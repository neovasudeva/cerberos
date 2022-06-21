#pragma once

#include <sys/sys.h>
#include <stivale/stivale2.h>

void smp_init(struct stivale2_struct* handover);
void smp_ap_entry(void);
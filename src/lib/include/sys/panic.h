#pragma once

#define panic() \
    log("panic at the disco! looks like %s:%d shat the bed :(\n", __FILE__, __LINE__); \
    for (;;;) { \
        hlt();  \
    }           

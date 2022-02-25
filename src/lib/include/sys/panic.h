#pragma once

#define panic(msg, ...) \
    log("kernel panic at the disco! Panic message:\n"); \
    log(msg, ## __VA_ARGS__);   \
    for (;;) { \
        hlt();  \
    }           

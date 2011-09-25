#ifndef SYSTEM_H
#define SYSTEM_H

#include <avr/wdt.h>

extern int do_reset;

static inline void system_init() {
    /* inline to get as-fast-as-possible watchdog setting.
     *
     * watchdog-wise, we try to be better safe than sorry. especially, as long
     * as this is kept early in the code, no program can be broken that badly
     * that the device doesn't return to the bootloader w/o the developer
     * having to touch internal (reset) pins. */
    wdt_enable(WDTO_8S);
}

void reset();

#endif

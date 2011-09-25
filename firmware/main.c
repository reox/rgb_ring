#include <avr/interrupt.h>
#include <util/delay.h>

#include "tlc594x.h"
#include "ledcontrol.h"

#include "system.h"
#include "usb.h"

void tick_10ms();

#include "milliseconds.c" 

int __attribute__((noreturn)) main(void) {
    system_init();
    tlc_setup();
    usb_init();
    sei();

    for(;;){
        usbPoll();
	if(do_reset == 1) {
		reset();
	}
	while (unhandled_10ms_ticks) {
		// should not loop more than once -- nothing should block the main loop for that long
		led_fade_step_all();
	}
	if (led_requires_retransmit) {
		tlc_send_blocking(led_value);
		led_requires_retransmit = 0;
	}
	wdt_reset();
    }
}

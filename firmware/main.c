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
	if (led_requires_retransmit) {
		tlc_send_blocking(led_value);
		led_requires_retransmit = 0;
	}
	_delay_ms(1);
	led_fade_step_all();
	wdt_reset();
    }
}

void tick_10ms()
{
}

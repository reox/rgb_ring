#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usbconfig.h"
#include "usbdrv.h"

#include "tlc594x.h"
#include "ledcontrol.h"

/*
 * if you want this to reset into the usbasploader, you need its bootLoaderInit
 * to set MCUCSR = 0 before wdt_disable and to really go to the bootloader if
 * MCUCSR has the WDRF'th bit set.
 * */
void reset() {
	usbDeviceDisconnect();
	wdt_enable(WDTO_15MS);
	wdt_reset();
	for(;;) _delay_ms(1);
}

int do_reset = 0;

static uchar buffer[64];

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
    usbMsgLen_t len = 0;
    usbRequest_t *rq = (void *)data;

    switch(rq->bRequest) {
	    case 1: // toggle led, return something meaningful in the buffer
		len = 3;
		if(len > rq->wLength.word) len = rq->wLength.word;
		usbMsgPtr = buffer;
		buffer[0] = led_value[16]>>4;
		buffer[1] = 42;
		buffer[2] = 23;
		return len;
	    case 2:
		led_requires_retransmit = 1;
		led_value[16] = 0;
		led_fade_to(16, 0xfff, 0x20);
		
		return 0;
	    case 3:
		len = 2;
		if(len > rq->wLength.word) len = rq->wLength.word;
		usbMsgPtr = buffer;
		buffer[0] = led_value[16]>>8;
		buffer[1] = led_value[16]&0xff;
		return len;
	    /*
	    case 2: // read stored data
		usbMsgPtr = stored_data + rq->wValue.word;
		if (usbMsgPtr >= &(stored_data[STORED_LENGTH])) return 0;
		return (rq->wLength.word + usbMsgPtr > &(stored_data[STORED_LENGTH])) ? &(stored_data[STORED_LENGTH]) - usbMsgPtr : rq->wLength.word;
	    case 3: // write stored data
		write_position = rq->wValue.word;
		write_remaining = (rq->wLength.word >= STORED_LENGTH - write_position) ? (STORED_LENGTH - write_position) : rq->wLength.word;
		return USB_NO_MSG;
		*/
	    case 255:
                do_reset = 1;
		return 0;
	    default:
		return 0;
    }
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
	/*
	uchar i;

	if (len > write_remaining) len = write_remaining;

	for (i = 0; i < len; ++i)
	{
		stored_data[write_position + i] = data[i];
	}

	write_remaining -= len;
	write_position += len;
	return write_remaining <= 0;
	*/
}

int __attribute__((noreturn)) main(void) {
    uchar i;

    wdt_disable();
    tlc_setup();
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        _delay_ms(1);
    }
    usbDeviceConnect();
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
    }
}

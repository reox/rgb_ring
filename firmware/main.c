#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usbconfig.h"
#include "usbdrv.h"

#include "tlc594x.h"

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
volatile int do_retransmit = 1;

static uchar buffer[64];
//uint16_t ledvalues[16*2] = {0x0, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800, 0x900, 0xa00, 0xb00, 0xc00, 0xd00, 0xe00, 0xfff, };
uint16_t ledvalues[16*2] = {0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff};

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
    usbMsgLen_t len = 0;
    usbRequest_t *rq = (void *)data;

    switch(rq->bRequest) {
	    case 1: // toggle led, return something meaningful in the buffer
		len = 3;
		if(len > rq->wLength.word) len = rq->wLength.word;
		usbMsgPtr = buffer;
		buffer[0] = TCNT0;
		buffer[1] = TIFR0;
		buffer[2] = 23;
		return len;
	    case 2:
		do_retransmit = 1;
		ledvalues[rq->wIndex.word] = rq->wValue.word << 4;
		return 0;
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
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        _delay_ms(1);
    }
    usbDeviceConnect();
    tlc_setup();
    sei();
    for(;;){
        usbPoll();
	if(do_reset == 1) {
		reset();
	}
	if (do_retransmit) {
		tlc_send_blocking(ledvalues);
		do_retransmit = 0;
	}
    }
}

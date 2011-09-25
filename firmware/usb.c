#include <util/delay.h>

#include "usb.h"
#include "system.h"

#include "ledcontrol.h"

uchar buffer[64];

int write_position, write_remaining;

void usb_init() {
    uchar i;

    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        _delay_ms(1);
    }
    usbDeviceConnect();
}

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
            case 's': // set all leds; this interface is just an immediate development aid, it's limited to 8bit channels and doesn't do the de-multiplexing in firmware
                write_position = rq->wIndex.word;
		write_remaining = (rq->wLength.word >= 32 - write_position) ? (32 - write_position) : rq->wLength.word;
                return USB_NO_MSG;
	    case 255:
                do_reset = 1;
		return 0;
	    default:
		return 0;
    }
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
        /* if you got here reading the code, you already know that this is just an intermediate step in code development */
	uchar i;

	if (len > write_remaining) len = write_remaining;

	for (i = 0; i < len; ++i)
	{
                // especially, this is asking for problems with fading; current fades will not be aborted
		led_value[write_position++] = data[i]<<4;
	}
        led_requires_retransmit = 1;

	write_remaining -= len;
	return write_remaining <= 0;
}

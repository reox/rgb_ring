#include <util/delay.h>

#include "usb.h"
#include "system.h"

#include "ledcontrol.h"

uchar buffer[64];

char last_command = 0;
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

    last_command = rq->bRequest;

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
            case 's':
		/* set leds directly; this interface is mainly for debugging as
		 * it doesn't do the de-multiplexing.
		 *
		 * data format:
		 * [ch0 high byte, ch0 low byte, ch1 high byte, ...]
		 *
		 * index specifies at which channel to start
		 */
                write_position = rq->wIndex.word * 2;
		write_remaining = (rq->wLength.word >= 32 - write_position) ? (32 - write_position) : rq->wLength.word;
                return USB_NO_MSG;
            case 'f':
		/* fade leds directly; this interface is mainly for debugging
		 * as it doesn't do the de-multiplexing.
		 *
		 * data format:
		 * [ch0 fade time high byte, ch1 fade tim low byte, ch0 high
		 * byte, ch0 low byte, ch1 fade time high byte, ...]
		 *
		 * index specifies at which channel to start. times are given
		 * in multiples of 10ms.
		 *
		 * be aware that for 2 16 channel chips, this already uses
		 * 128byte of usb message data; larger devices will need large
		 * usb message size enabled.
		 */
                write_position = rq->wIndex.word * 4;
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
	uchar i;
	static uint8_t set_channel;
	static uint16_t set_value;
	static uint16_t set_time;

	if (len > write_remaining) len = write_remaining;

	switch(last_command) {
	case 's':
		for (i = 0; i < len; ++i)
		{
			if (i%2 == 0) {
				set_channel = write_position/2;
				set_value = data[i] << 8;
			} else {
				led_set_to(set_channel, set_value | data[i]);
			}

			write_position += 1;
		}
		if (write_remaining == 0)
			led_requires_retransmit = 1;
		break;
	case 'f':
		for (i = 0; i < len; ++i)
		{
			switch (i%4) {
			case 0:
				set_channel = write_position/4;
				set_time = data[i] << 8;
				break;
			case 1:
				set_time |= data[i];
				break;
			case 2:
				set_value = data[i] << 8;
				break;
			case 3:
				led_fade_to(set_channel, set_value | data[i], set_time);
				break;
			}

			write_position += 1;
		}
		if (write_remaining == 0)
			led_requires_retransmit = 1;
		break;
	}

	write_remaining -= len;
	return write_remaining <= 0;
}

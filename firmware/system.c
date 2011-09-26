#include "system.h"

#include "usbconfig.h"
#include "usbdrv.h"

int do_reset = 0;

uint16_t millis = 0;

/*
 * if you want this to reset into the usbasploader, you need its bootLoaderInit
 * to set MCUCSR = 0 before wdt_disable and to really go to the bootloader if
 * MCUCSR has the WDRF'th bit set.
 * */
void reset() {
	usbDeviceDisconnect();
	wdt_enable(WDTO_15MS);
	wdt_reset();
	for(;;);
}

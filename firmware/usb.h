#ifndef USB_H
#define USB_H

#include "usbconfig.h"
#include "usbdrv.h"

extern uchar buffer[64];

void usb_init();
usbMsgLen_t usbFunctionSetup(uchar data[8]);
uchar usbFunctionWrite(uchar *data, uchar len);

#endif

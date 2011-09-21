#!/usr/bin/env python

import time

from device import discover
import usb

my_handle = discover()

for x in range(32):
    data = my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_IN, 2, 0, index=x, value=0)
    time.sleep(0.2)
print data

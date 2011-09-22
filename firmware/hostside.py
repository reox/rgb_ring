#!/usr/bin/env python

import time

from device import discover
import usb

my_handle = discover()

data = my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_IN, 2, 40)
print data
print [a<<8|b for (a, b) in zip(data[::2], data[1::2])]

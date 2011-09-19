#!/usr/bin/env python

import time

from device import discover
import usb

my_handle = discover()

data = my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_IN, 1, 3)
print data

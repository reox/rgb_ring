#!/usr/bin/env python

import usb
from device import discover

my_handle = discover()

try:
    my_handle.controlMsg(usb.TYPE_VENDOR, 255, 0)
except usb.USBError:
    pass # well, it's resetting...

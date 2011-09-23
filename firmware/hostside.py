#!/usr/bin/env python

import time

from device import discover
import usb
import Image

def set_rgb(data):
    # until we've got the de-multiplexing in avr, we're doing it here
    # minimal modification: compensate for the unconnected pin; everything else is color and led permutation
    data[16:16] = [0,]
    set_leds(data)

def set_leds(data):
    my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_OUT, ord('s'), data, index=0)

def send_image(filename):
    i = Image.open(filename)
    assert i.size[0] == 10, "Wrong image size"
    for y in range(i.size[1]):
        linedata = [0]*i.size[0]*3
        for x in range(i.size[0]):
            linedata[3*x:3*(x+1)] = i.getpixel((x, y))
        set_rgb(linedata)
        time.sleep(0.1)

my_handle = discover()

data = my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_IN, 2, 40)
print data
print [a<<8|b for (a, b) in zip(data[::2], data[1::2])]

time.sleep(3) # wait until the led is faded; currently, fading is not aborted when explicitly setting leds

send_image('./demo.png')

#!/usr/bin/env python

import time
import itertools

from device import discover
import usb
import Image

def set_rgb(data):
    # until we've got the de-multiplexing in avr, we're doing it here
    # minimal modification: compensate for the unconnected pin; everything else is color and led permutation
    data[16:16] = [0,]
    set_leds(data)

def set_leds(data):
    # 20*10ms fade time, data shifted to 16bit level
    data = itertools.chain(*zip([0]*len(data), [20]*len(data), data, [0]*len(data)))
    my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_OUT, ord('f'), list(data)[4*16:], index=16)

def send_image(filename):
    i = Image.open(filename)
    assert i.size[0] == 10, "Wrong image size"
    for y in range(i.size[1]):
        linedata = [0]*i.size[0]*3
        for x in range(i.size[0]):
            linedata[3*x:3*(x+1)] = i.getpixel((x, y))
        set_rgb(linedata)
        time.sleep(0.2)

def single_command(command):
    data = my_handle.controlMsg(usb.TYPE_VENDOR | usb.RECIP_DEVICE | usb.ENDPOINT_IN, command, 40)
    print data
    print [a<<8|b for (a, b) in zip(data[::2], data[1::2])]

my_handle = discover()

single_command(1) # show nice 23/42 to show it's our firmware

single_command(2) # fade led
time.sleep(0.1)
single_command(3) # show current led fade status
time.sleep(0.1)
single_command(3) # show current led fade status

time.sleep(3) # wait until the led is faded; currently, fading is not aborted when explicitly setting leds

send_image('./demo.png')

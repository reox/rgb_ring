bresenham_demo: bresenham_demo.c

bresenham_demo.bin: bresenham_demo.c
	avr-gcc bresenham_demo.c -mmcu=atmega168 -o bresenham_demo.bin -DAVR_STDOUT -DF_CPU=16000000

%.hex: %.bin
	rm -f $@ $*.eep.hex
	avr-objcopy -j .text -j .data -O ihex $< $@

arduino_bresenham_demo_upload: bresenham_demo.hex
	avrdude -c avrisp -b 19200 -P /dev/arduino -p atmega168 -U flash:w:$<:i -F

clean:
	rm -f *.o *.bin *.hex bresenham_demo

F_CPU = 16000000
MCU=atmega168

COMPILE = avr-gcc -Wall -Os -I. -Iusbdrv -mmcu=${MCU} -DF_CPU=${F_CPU} #-DBAUD=${BAUD}

OBJECTS = main.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o tlc594x.o ledcontrol.o

# symbolic targets:
all:	main.hex

AVRDUDE = avrdude -c usbasp -p atmega168

upload: all uploadable
	$(AVRDUDE) -U flash:w:main.hex:i

uploadable:
	python reset.py && sleep 1.5 || true

boot: uploadable
	$(AVRDUDE)

prompt: uploadable
	$(AVRDUDE) -t

%.o: %.c usbconfig.h usbdrv
	$(COMPILE) -c $< -o $@

%.o: %.S usbconfig.h usbdrv
	$(COMPILE) -c $< -o $@

usbdrv:
	echo "Please copy a current usbdrv directory here."
	echo "It can be obtained from:"
	echo "http://www.obdev.at/products/vusb/"
	exit 1

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s

main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS)

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex

disasm:	main.bin
	avr-objdump -d main.bin

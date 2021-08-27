# Makefile for building the source code
MCU=attiny13
FUSE_L=0x2A
FUSE_H=0xFB
F_CPU=1200000
CC=avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
AVRDUDE=avrdude
CFLAGS=-flto -std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.
TARGET=74hc595

SRCS = 74hc595.c 
	
all:
	${CC} ${CFLAGS} -o ${TARGET} ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET} ${TARGET}.hex	

flash:
	${AVRDUDE} -p ${MCU} -c usbasp -U flash:w:${TARGET}.hex:i -F -P usb

fuse:
	$(AVRDUDE) -p ${MCU} -c usbasp -U hfuse:w:${FUSE_H}:m -U lfuse:w:${FUSE_L}:m
	
clean:
	rm -f *.c~ *.hex $(TARGET)

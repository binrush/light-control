BAUD=19200
SRC=main
CONTROLLER=atmega8
F_CPU=1000000 # 1MHz for accurate baudrate timing
ISPDEV=/dev/ttyUSB003
ISPTYPE=arduino

CFLAGS=-mmcu=$(CONTROLLER) -g -DF_CPU=$(F_CPU) -Wall -Os -Werror -Wextra -std=c99
LDFLAGS=-mmcu=$(CONTROLLER)
CC=avr-gcc

hex: $(SRC)
	avr-objcopy -j .text -j .data -O ihex $(SRC) $(SRC).flash.hex

$(SRC): main.o messages.o timers.o

upload: hex
	avrdude -P /dev/ttyACM0 -b $(BAUD) -c avrisp -p m8 -U flash:w:$(SRC).flash.hex:i

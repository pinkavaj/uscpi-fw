#####         Target Specific Details          #####
#####     Customize these for your project     #####

# port where JTAG programmer is connected
PROG_PORT=/dev/ttyUSB1

# Name of target controller
# (see the available avr-gcc mmcu options for possible values)
MCU=atmega162
# CPU Frequency, more at avrprog boot library README
F_CPU = 11059200
PROJECT=uSCPIfw

# Source files
# List C/C++/Assembly source files:
# (list all files to compile, e.g. 'a.c b.cpp as.S')
# Use .cc, .cpp or .C suffix for C++ files, use .S
# (NOT .s !!!) for assembly source code files.
#INC="-I. -I/usr/lib/avr/include/"
INC="-I. -I/usr/lib/avr/include/"
SRC=-c main.c -c scpi.c -c usart.c -c timer.c -c spi.c
#OBJ=main.o scpi.o
OBJ=main.o
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

# optimize for size:
CFLAGS=-g --std=gnu99 -Os -mmcu=$(MCU) -mrelax \
	-Wall -Wstrict-prototypes -Wstrict-aliasing=1 -Wextra -pedantic \
	-ffreestanding -fshort-enums -fstrict-aliasing -funsigned-char \
	${INC} -DF_CPU=${F_CPU} \
	--combine # -fwhole-program 
# 
# experimental stuff
# 
# assembler generate line numbers
# -Wa,-gstabs
#  Not harmfull
# -finline-functions 
#  Make code a little bigger, but might be usefull
#  -mcall-prologues
# Deprecated, not working, broke program
#CFLAGS+= --combine -fwhole-program -funsigned-bitfields

all: hex

load: hex
	avrdude -p m162 -c avr910 -b 115200 -P ${PROG_PORT} \
		-U flash:w:${PROJECT}.hex:i 2>&1 || true

hex: link
	$(OBJCOPY) -R .eeprom -O ihex ${PROJECT}.out ${PROJECT}.hex

link: build
	$(CC) $(CFLAGS) ${OBJ} -o ${PROJECT}.out

build: ver
	$(CC) $(CFLAGS) ${SRC}

ver:
	sed -i "s/VER_BUILD .*/VER_BUILD `svnversion | sed "s/.*://"`/" \
		config.h || true

disas:
	$(OBJDUMP) -d ${PROJECT}.out

size:
	size ${PROJECT}.out

clean:
	rm *.o *.out *.hex


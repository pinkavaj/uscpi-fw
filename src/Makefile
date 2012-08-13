#####         Target Specific Details          #####
#####     Customize these for your project     #####

# port where JTAG programmer is connected
PROG_PORT=/dev/ttyS0
PROG_BAUDRATE=115200

#PROG_PORT=/dev/ttyUSB0
#PROG_BAUDRATE=19200

DEV_PORT=/dev/ttyUSB0

# Name of target controller
# (see the available avr-gcc mmcu options for possible values)
MCU=atmega162
PROJECT=uSCPIfw

# Source files
# List C/C++/Assembly source files:
# (list all files to compile, e.g. 'a.c b.cpp as.S')
# Use .cc, .cpp or .C suffix for C++ files, use .S
# (NOT .s !!!) for assembly source code files.
INC=-I. -Iinclude -I/usr/lib/avr/include/ -Iscpi/include

SRC=-c main.c
SRC+=lib/iobuf.c \
     lib/math_cust.S \
     lib/pic16.c \
     lib/thermometer_pt.c
SRC+=scpi/scpi.c \
     scpi/func/temp.c \
     scpi/func/list.c \
     scpi/cmd_tools.c \
     scpi/cmd_tree.c \
     scpi/cmd/cc.c \
     scpi/cmd/ic_cont.c \
     scpi/cmd/ic_meas.c \
     scpi/cmd/ic_test.c \
     scpi/cmd/ic_sens_list.c \
     scpi/cmd/ic_sour_temp.c \
     scpi/cmd/ic_stat.c \
     scpi/cmd/ic_syst.c \
     scpi/key.c \
     scpi/stat_report.c
SRC+=drivers/usart.c drivers/timer.c
SRC+=drivers/spi_dev.c \
	 drivers/spi.c \
	 drivers/ad974.c \
	 drivers/valves.c \
	 drivers/valves_raw.c

#	 drivers/mcp4922.c \
#	 drivers/max1068.c \
#	 drivers/tlv5610.c \

OBJ=main.o math_cust.o
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

# optimize for size:
CFLAGS=-g --std=gnu99 -Os -mmcu=$(MCU) -mrelax \
	-Wall -Wstrict-prototypes -Wstrict-aliasing=1 -Wextra -pedantic \
	-ffreestanding -fshort-enums -fstrict-aliasing -funsigned-char \
	-funsigned-bitfields ${INC} \
	--combine -fwhole-program 
#debug
#CFLAGS=-g --std=gnu99 -O1 -mmcu=$(MCU) -mrelax ${INC} -DF_CPU=${F_CPU} --combine

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
#CFLAGS+= --combine -fwhole-program 

all: hex

load: hex
	avrdude -p m162 -c avr910 -b ${PROG_BAUDRATE} -P ${PROG_PORT} \
		-U flash:w:${PROJECT}.hex:i 2>&1 | \
		grep -v "Device code" || true

pass:
	avrdude -p m162 -c avr910 -b ${PROG_BAUDRATE} -P ${PROG_PORT} \
		-U eeprom:w:${PROJECT}_eeprom.hex:i 2>&1 | \
		grep -v "Device code" || true

hex: link
	$(OBJCOPY) -R .eeprom -O ihex ${PROJECT}.out ${PROJECT}.hex
	$(OBJCOPY) -j .eeprom -O ihex ${PROJECT}.out ${PROJECT}_eeprom.hex

link: build
	$(CC) $(CFLAGS) ${OBJ} -o ${PROJECT}.out

build: ver clean
	$(CC) $(CFLAGS) ${SRC}

ver:
	sed -i "s/VER_BUILD .*/VER_BUILD `svnversion | sed "s/.*://"`/" \
		config.h || true

dump:
	$(OBJDUMP) --source -d ${PROJECT}.out | vimpager -c "set syntax=asm" -

size:
	size ${PROJECT}.out

clean:
	find .  -name "*.hex" -exec rm \{\} \;
	find .  -name "*.o" -exec rm \{\} \;
	find .  -name "*.out" -exec rm \{\} \;

test:
	cd ../test && make

pttable:
	python3 ../../dhcu_2.0/tools/pt_table.py >lib/thermometer_pt.c
	grep -A1000 "begin of header" lib/thermometer_pt.c | \
		grep -B1000 "end of header" | \
		grep -v " of header" >include/lib/thermometer_pt.h


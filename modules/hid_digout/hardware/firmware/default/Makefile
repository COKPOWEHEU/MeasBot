###############################################################################
# Makefile for the project main
###############################################################################

## General Flags
PROJECT = main
MCU = atmega8
TARGET = main.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -I.. -I../usbdrv
CFLAGS += -Wall -gdwarf-2 -std=gnu99    -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
#LDFLAGS +=  -Wl,-Map=main.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = main.o usbdrv.o usbdrvasm.o oddebug.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) main.hex main.eep main.lss size

## Compile
%.o: ../%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

%.o: ../usbdrv/%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

%.o: ../usbdrv/%.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) main.elf dep/* main.hex main.eep main.lss main.map
prog:
	avrdude -c stk500v2 -P avrdoper -p $(MCU) -U flash:w:main.hex:i
ftdi:
	avrdude -c 4232h -p $(MCU) -U flash:w:main.hex:i
#MISO = 2 / RTS
#MOSI = 1 / Rx
#SCK = 0 / Tx
#RST = 3 / CTS
#(это не работает, buff берем с RST) buff = ~4 / DTR (1-idle, 0-work)


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)


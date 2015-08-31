TARG = pcm2705

MCU = atmega8
F_CPU = 1000000

KS0066_WIRE = KS0066_WIRE_PCF8574

# Source files
SRCS = $(wildcard *.c)

# Compiler options
OPTIMIZE = -O2 -mcall-prologues -fshort-enums
DEBUG = -g -Wall -Werror
CFLAGS = $(DEBUG) -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS = $(DEBUG) -mmcu=$(MCU)

# AVR toolchain
CC       = avr-gcc
OBJCOPY  = avr-objcopy

# Avrdude flasher
AVRDUDE  = avrdude
AD_MCU = -p $(MCU)
#AD_PROG = -c avr109
#AD_PORT = -P /dev/ttyACM0

AD_CMDLINE = $(AD_MCU) $(AD_PROG) $(AD_PORT)

# Linker options
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
ELF = $(OBJDIR)/$(TARG).elf

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS) -lm
	mkdir -p flash
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram $(ELF) flash/$@.hex
	./size.sh $(ELF)

obj/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D$(KS0066_WIRE) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)

flash: $(TARG)
	$(AVRDUDE) $(AD_CMDLINE) -U flash:w:flash/$(TARG).hex:i

eeprom:
	$(AVRDUDE) $(AD_CMDLINE) -U eeprom:w:eeprom.bin:r

fuse:
	$(AVRDUDE) $(AD_CMDLINE) -U lfuse:w:0xA1:m -U hfuse:w:0xD1:m

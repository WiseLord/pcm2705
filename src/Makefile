# Output file name
TARG     = pcm2705

# MCU name and frequency
MCU      = atmega8
F_CPU    = 8000000

# Source files
SRCS     = $(wildcard *.c)

# Build directory
BUILDDIR = build

# Compiler options
OPTIMIZE = -Os -mcall-prologues -fshort-enums -ffunction-sections -fdata-sections -ffreestanding -flto
WARNLEVEL = -Wall
CFLAGS = $(WARNLEVEL) -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += -MMD -MP -MT $(BUILDDIR)/$(*F).o -MF $(BUILDDIR)/$(*D)/$(*F).d
LDFLAGS = $(WARNLEVEL) -mmcu=$(MCU) -Wl,--gc-sections -Wl,--relax

# AVR toolchain and flasher
PREFIX = avr-
CC = $(PREFIX)gcc
CP = $(PREFIX)objcopy
OD = $(PREFIX)objdump
SZ = $(PREFIX)size

# AVRDude parameters
AVRDUDE  = avrdude
AD_MCU   = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMDLINE   = $(AD_MCU) $(AD_PROG) $(AD_PORT) -V

OBJS = $(addprefix $(BUILDDIR)/, $(SRCS:.c=.o))
ELF = $(BUILDDIR)/$(TARG).elf
HEX = flash/$(TARG).hex

all: $(HEX) size

$(HEX): $(ELF)
	$(CP) -O ihex -R .eeprom -R .nwram $(ELF) $(HEX)

$(ELF): $(OBJS)
	@mkdir -p $(addprefix $(BUILDDIR)/, $(SUBDIRS)) flash
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS)
	$(OD) -h -S $(ELF) > $(BUILDDIR)/$(TARG).lss

size: $(ELF)
	$(SZ) $(ELF)

$(BUILDDIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEFINES) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: flash
flash: $(HEX)
	$(AVRDUDE) $(AD_CMDLINE) -U flash:w:flash/$(TARG).hex:i

.PHONY: eeprom
eeprom:
	$(AVRDUDE) $(AD_CMDLINE) -U eeprom:w:eeprom/$(TARG).bin:r

.PHONY: fuse
fuse:
	$(AVRDUDE) $(AD_CMDLINE) -U lfuse:w:0xA4:m -U hfuse:w:0xD1:m

# Dependencies
-include $(OBJS:.o=.d)

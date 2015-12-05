TARG=pcm2705

MCU = atmega8
F_CPU = 1000000L

# Source files
SRCS = $(wildcard *.c)

BUILDDIR = build

# Compiler options
OPTIMIZE = -Os -mcall-prologues -fshort-enums -ffunction-sections -fdata-sections
DEBUG = -g -Wall -Werror
CFLAGS = $(DEBUG) -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS += -MMD -MP -MT $(BUILDDIR)/$(*F).o -MF $(BUILDDIR)/$(@F).d
LDFLAGS = $(DEBUG) -mmcu=$(MCU) -Wl,-gc-sections

# AVR toolchain and flasher
CC = avr-gcc
OBJCOPY = avr-objcopy

AVRDUDE = avrdude
AD_MCU = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMDLINE = $(AD_MCU) $(AD_PROG) $(AD_PORT) -B 20 -V

OBJS = $(addprefix $(BUILDDIR)/, $(SRCS:.c=.o))
ELF = $(BUILDDIR)/$(TARG).elf

all: $(ELF) size

$(ELF): $(OBJS)
	$(CC) $(LDFLAGS) -o $(ELF) $(OBJS) -lm
	@mkdir -p flash
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram $(ELF) flash/$(TARG).hex

size:
	@./size.sh $(ELF)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -D$(MCU) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: flash
flash: $(ELF)
	$(AVRDUDE) $(AD_CMDLINE) -U flash:w:flash/$(TARG).hex:i

eeprom:
	$(AVRDUDE) $(AD_CMDLINE) -U eeprom:w:eeprom.bin:r

.PHONY: fuse
fuse:
	$(AVRDUDE) $(AD_CMDLINE) -U lfuse:w:0xA1:m -U hfuse:w:0xD1:m

# Other dependencies
-include $(wildcard $(BUILDDIR)/*.d)

# The name of the project
NAME = robox

# The directory to store binary files into
BINS = bin

# The binaries that can be created
BIN = $(BINS)/$(NAME).bin
HEX = $(BINS)/$(NAME).hex
ELF = $(BINS)/$(NAME).elf

# The sources that will be used
SRC = $(wildcard *.c)

# The objects that will be created
OBJ = $(SRC:%.c=$(BINS)/%.o)

# Files containing dependencies between source and header files
DEP = $(OBJ:%.o=%.d)

# Flags that will be passed to GCC
CFLAGS = -mmcu=attiny461a -mint8 -Os -Wall -Wextra -Werror -std=c99 -flto \
	-DF_CPU=8000000
	
#-gstabs
# -L/home/www/avr8-gnu-toolchain-linux_x86_64/avr/lib/avr25

# -Wl,-u,vfprintf -lprintf_min -lprintf_min

.PHONY: all clean flash mk2 help

all: elf
elf: $(ELF)
hex: $(HEX)
bin: $(BIN) 
clean:
	rm -f $(BIN) $(HEX) $(ELF) $(OBJ) $(DEP)
	
flash: $(ELF)
	avrdude -p t461 -c stk500 -P /dev/ttyUSB0 -U flash:w:$(ELF) -v

mk2: $(ELF)
	avrdude -c avrisp2 -p t461 -v -U flash:w:$(ELF)

help:
	@echo "Targets:"
	@echo "  all   - (default) Builds the firmware. Same as the elf target"
	@echo "  bin   - Produces a bin file"
	@echo "  clean - Cleans the compiled objects"
	@echo "  elf   - Produces an ELF file"
	@echo "  flash - Flash the project onto a chip"
	@echo "  help  - Shows this help file"
	@echo "  hex   - Produces a hex file"

$(ELF): $(OBJ)
	avr-gcc $(CFLAGS) -o $@ $^
	avr-size $@

$(HEX): $(ELF)
	avr-objcopy -O binary $(ELF) $(HEX)
	
$(BIN): $(ELF)
	avr-objcopy -O ihex $(ELF) $(BIN)

$(BINS)/%.o: %.c $(BINS)/.create
	avr-gcc $(CFLAGS) -MMD -MP -c -o $@ $<

$(BINS)/.create:
	mkdir -p $(BINS)/.create

-include $(DEP)

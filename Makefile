# The name of the project
NAME = blspeak

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
CFLAGS = -mmcu=attiny461a -mint8 -Os -Wall -Wextra -std=c99 -flto

.PHONY: all clean help

all: $(ELF)
clean:
	rm -f $(BIN) $(HEX) $(ELF) $(OBJ) $(DEP)

help:
	@echo "Targets:"
	@echo "  all   - Builds the firmware"
	@echo "  clean - Cleans the compiled objects"
	@echo "  help  - Shows this help file"

$(ELF): $(OBJ)
	avr-gcc $(CFLAGS) -o $@ $^

$(BINS)/%.o: %.c $(BINS)/.create
	avr-gcc $(CFLAGS) -MMD -MP -c -o $@ $<

$(BINS)/.create:
	mkdir -p $(BINS)/.create

-include $(DEP)

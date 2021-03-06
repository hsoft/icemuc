MCU ?= attiny45
F_CPU ?= 1000000UL
CC = avr-gcc
CFLAGS = -Os -Wall -DF_CPU=$(F_CPU) -mmcu=$(MCU) -c
LDFLAGS = -mmcu=$(MCU)
OBJS ?= main.o layer.o

AVRDUDEMCU = t45
AVRDUDEARGS = -c usbtiny -P usb 

SIM_OBJS ?= main.o sim.o
NCURSES_LINKING = `pkg-config --libs --static ncurses`

# Patterns

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Rules

.PHONY: send clean

all: $(PROGNAME).hex
	@echo Done!

send: $(PROGNAME).hex
	avrdude $(AVRDUDEARGS) -p $(AVRDUDEMCU) -U flash:w:$(PROGNAME).hex

clean:
	rm -f $(OBJS) $(PROGNAME).hex $(PROGNAME).bin $(SIM_OBJS) $(PROGNAME)

$(PROGNAME).bin: $(OBJS)
	$(CC) $(LDFLAGS) $+ -o $@

$(PROGNAME).hex: $(PROGNAME).bin
	avr-objcopy -O ihex -R .eeprom $< $@

# Simulation

.PHONY: sim

# we always want to run make on the ../.. to make sure we have a fresh lib.
$(PROGNAME): $(SIM_OBJS)
	$(MAKE) -C ../..
	$(CC) -L../.. $(SIM_OBJS) -o $@ -licemu $(NCURSES_LINKING)

sim: CC = cc
sim: CFLAGS = -DSIMULATION -Wall `pkg-config --cflags ncurses` -c
sim: $(PROGNAME)
	@echo Done!


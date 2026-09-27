# Garage_Door_V2 - ATtiny26, avr-gcc
# make          -> Garage_Door_V2.hex and Garage_Door_V2.eep
# make flash    -> write flash only (adjust ISP programmer), taught-in code is only kept with EESAVE
# make eeprom   -> set EEPROM to 0 (= not coded, teach in again afterwards)
# make fuses    -> internal RC oscillator 8 MHz, EESAVE set

MCU        = attiny26
F_CPU      = 8000000UL
PROGRAMMER = usbasp
TARGET     = Garage_Door_V2
SRC        = Garage_Door_V2.cpp Debounce_Button.cpp Detect_Edge.cpp Code_Entry.cpp \
             Code_Check.cpp Drive_Outputs.cpp Blink_Clock.cpp

CXX      = avr-g++
OBJCOPY  = avr-objcopy
CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS  = -mmcu=$(MCU) -Wl,--gc-sections

OBJ = $(SRC:.cpp=.o)

all: $(TARGET).hex $(TARGET).eep

$(TARGET).elf: $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^
	avr-size $@

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).eep: $(TARGET).elf
	$(OBJCOPY) -O ihex -j .eeprom --change-section-lma .eeprom=0 $< $@

flash: all
	avrdude -p t26 -c $(PROGRAMMER) -U flash:w:$(TARGET).hex:i

eeprom: all
	avrdude -p t26 -c $(PROGRAMMER) -U eeprom:w:$(TARGET).eep:i

fuses:
	avrdude -p t26 -c $(PROGRAMMER) -U lfuse:w:0xE4:m -U hfuse:w:0xF3:m

clean:
	rm -f *.o *.elf *.hex *.eep

.PHONY: all flash eeprom fuses clean

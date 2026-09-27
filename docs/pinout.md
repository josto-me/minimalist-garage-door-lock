# Pinout and wiring

## V2 – ATtiny26

DIP-20 pin numbers refer to the ATtiny26 DIP package.

| Signal | Port | DIP-20 pin | Direction | Active level | Used in firmware |
|---|---|---|---|---|---|
| Relay door 1 | PA7 | 11 | output | LOW = relay on | yes |
| Relay door 2 | PA6 | 12 | output | LOW = relay on | yes |
| White LED "ackn." (feedback blinking) | PB3 | 4 | output | HIGH = on | yes |
| Relay supply transistor (BC547B) | PB4 | 7 | output | HIGH = relay supply on | yes |
| Coding LED (re-coding mode) | PB5 | 8 | output | HIGH = on | yes |
| Code button | PA3 | 17 | input, pull-up | LOW = pressed | yes |
| Jumper 1 "recode" | PA5 | 13 | input, pull-up | LOW = jumper set | yes |
| Jumper 2 | PA4 | 14 | input | – | no |
| Jumper 3 | PA2 | 18 | input | – | no |
| Coding button | PA1 | 19 | input, pull-up | LOW = pressed | no (configured only) |

PB4 and PB5 are the XTAL pins of the ATtiny26. They can only be used as I/O when the chip runs from its internal RC oscillator. The firmware is set for 8 MHz: timer tick 8 MHz / 64 / 256 = 2.048 ms.

### Parts list

| Qty | Part |
|---|---|
| 1 | ATtiny26-16PU (DIP-20) |
| 2 | push button (code button, coding button) |
| 1 | transistor BC547B (relay supply) |
| 1 | white LED (5 V type) |
| 3 | jumpers |
| 1 | 2×3 pin header |
| 1 | USB cable |

Also needed: two relays with active-low inputs (e.g. a 2-channel relay module), a base resistor for the BC547B, and the coding LED with a series resistor. Choose the values for your relays and LEDs.

### V2 wiring

| Net | Connects | Note |
|---|---|---|
| VCC | ATtiny26 pin 5 (VCC), pin 15 (AVCC), relay supply switch, LED anodes | 5 V from the USB cable |
| GND | ATtiny26 pin 6 (GND), pin 16 (AGND), button and jumper commons, BC547B emitter | |
| BUTTON | PA3 (pin 17) – code button – GND | internal pull-up, no external resistor needed |
| JUMPER_1 | PA5 (pin 13) – jumper – GND | set = re-coding mode |
| JUMPER_2 / JUMPER_3 | PA4 (pin 14), PA2 (pin 18) – jumper – GND | unused by the firmware |
| BUTTON_CODING | PA1 (pin 19) – button – GND | unused by the firmware |
| LED_ACKN | PB3 (pin 4) – white LED – GND | a "5 V" LED has a built-in series resistor; otherwise add one |
| LED_CODING | PB5 (pin 8) – LED + series resistor – GND | |
| RELAY_VCC | PB4 (pin 7) – base resistor – BC547B base | the transistor switches the relay supply, HIGH = on |
| DOOR_1 | PA7 (pin 11) – relay input door 1 | LOW = relay on (active-low relay input) |
| DOOR_2 | PA6 (pin 12) – relay input door 2 | as DOOR_1 |
| ISP | PB0 MOSI (1), PB1 MISO (2), PB2 SCK (3), PB7 RESET (10), VCC, GND – 2×3 header | wire as a standard 6-pin AVR ISP connector |

Wire each relay contact in parallel to the door opener push button of its garage door. The firmware closes the contact for a 0.5 s pulse.

## V1 – Arduino Mega 2560

| Signal | Port | Arduino pin | Active level |
|---|---|---|---|
| Code button | PA0 | D22 | LOW = pressed (internal pull-up) |
| Relay door 1 | PA1 | D23 | LOW = relay on |
| Relay supply | PA2 | D24 | HIGH = supply on, switched off after 30 s without input |
| Status LED | PA3 | D25 | HIGH = on |

## V3 – Home_Control (Arduino Mega 2560)

| Signal | Port | Arduino pin | Active level |
|---|---|---|---|
| Code button | PA0 | D22 | LOW = pressed (internal pull-up) |
| LED | PG2 | D39 | HIGH = on |
| Relay supply | PC7 | D30 | HIGH = supply on |
| Relay door 1 | PC6 | D31 | LOW = relay on |
| Relay door 2 | PC5 | D32 | LOW = relay on |

V3 uses AVR registers of the ATmega2560 directly (`DDRA`, `PORTG`, `PORTC`, `TCCR0A/B`, `TIMSK0`, `TIMER0_OVF_vect`) and runs only on the Arduino Mega 2560.

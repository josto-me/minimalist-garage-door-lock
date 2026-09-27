// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Config.h
 *
 *  Author: Johannes Stockhammer
 *
 * Common defines for all modules: clock, pins, layout of the numeric code.
 * The numeric code itself is NOT in the program but in the EEPROM
 * (it is taught in via the jumper "Recode").
 */


#ifndef CONFIG_H_
#define CONFIG_H_

//Includes
#include <stdint.h>
#include <avr/io.h>

//Defines
	//Clock
#ifndef F_CPU
#define F_CPU 8000000UL					//internal RC oscillator 8 MHz (set to the clock of your board)
#endif
#define TIMER0_PRESCALER 64				//TCCR0 = 0b00000011
#define TICK_US ((256UL*TIMER0_PRESCALER*1000UL)/(F_CPU/1000UL))	//Timer0 overflow: 256 * 64 / 8MHz = 2048µs
#define MS_TO_TICKS(ms) ((((uint32_t)(ms)*1000UL)+(TICK_US/2))/TICK_US)	//Convert ms to ISR runs (rounded)

	//Inputs/Outputs
		//Outputs
#define BIT_DOOR_1 PA7					//Relay door 1 (LOW = relay energized)
#define BIT_DOOR_2 PA6					//Relay door 2 (LOW = relay energized)
#define BIT_LED_ACKN PB3				//white LED, acknowledgement (blink clock)
#define BIT_RELAY_VCC PB4				//Transistor BC547B, HIGH = relay supply on
#define BIT_LED_CODING PB5				//LED coding, lit as long as the jumper recode is set
		//Inputs (all with internal pull-up, LOW = active)
#define BIT_CODE_BUTTON PA3				//Code button (control panel)
#define BIT_JUMPER_RECODE PA5			//Jumper 1 = recode
#define BIT_BUTTON_CODING PA1			//Button coding (prepared, not evaluated in the program)

	//Logic
#define ON 1
#define OFF 0

	//Numeric code
#define DIGIT_DOOR 0					//1st digit = door selection (1 or 2)
#define DIGIT_CODE_1 1					//the code digits follow from here
#define NUM_DIGITS 5					//door selection + 4 code digits
#define NUM_DOORS 2						//door 1 and door 2
#define NUMBER_MAX 255					//pulses per digit, counter stops here (no overflow to 0)
#define CODE_EMPTY_0 0					//EEPROM start value -> not coded yet
#define CODE_EMPTY_FF 0xFF				//erased EEPROM -> not coded yet

#endif /* CONFIG_H_ */

// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Garage_Door_V2.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Version:      2.0
 * Hardware:     ATtiny26 (internal RC oscillator 8 MHz), button, jumper recode,
 *               BC547B for the relay supply, 2 relays (LOW = energized), LEDs
 * Software:     AVR-GCC (avr-g++), see Makefile
 * Description:  Garage door code lock with one button. Each digit is entered as the
 *               number of button presses, a pause completes the digit.
 *               1st digit = door (1 or 2), then 4 code digits. Code correct -> relay
 *               of the door energizes briefly. The code is stored in the EEPROM; with the
 *               jumper "recode" set, a new code is taught in.
 */

//Includes
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "Config.h"
#include "Debounce_Button.h"
#include "Detect_Edge.h"
#include "Code_Entry.h"
#include "Code_Check.h"
#include "Drive_Outputs.h"
#include "Blink_Clock.h"

//Defines
	//Sequence operate
#define OPERATE_ENTRY 0
#define OPERATE_CHECK 1
#define OPERATE_DRIVE_OUTPUTS 2
	//Sequence coding
#define CODING_START 0
#define CODING_ENTRY 1
#define CODING_SAVE 2
#define CODING_FINISHED 3
	//Acknowledgement
#define BLINK_CODE_WRONG 5							//Blink 5x on wrong code
#define BLINK_CODE_SAVED 10							//Blink 10x after recoding

//Variables
	//Timers (counted down in the ISR, 8 bit -> access is atomic)
volatile uint8_t debounce_timer_panel_button=0;
volatile uint8_t debounce_timer_jumper_recode=0;
volatile uint8_t entry_timer=0;
volatile uint8_t timer_drive_outputs=0;
volatile uint8_t blink_timer=0;
	//Button and jumper
bool panel_button_state=OFF, panel_button_debounce=0, panel_button_state_before=OFF, panel_button_edge_low_high=0;
bool jumper_recode_state=OFF, jumper_recode_debounce=0;
	//Sequence
uint8_t sequence_operate=OPERATE_ENTRY, sequence_coding=CODING_START;
bool code_entry_finished=0, code_entry_reset=0, entry_code_ok=0, code_check_finished=0, drive_outputs_finished=0;
	//Blink clock
uint8_t num_blinks=0;
bool blink_clock_finished=1;						//1 = no blink clock active
	//Code
uint8_t entry_array[NUM_DIGITS]={0,0,0,0,0};		//entered numbers, digit 0 = door
uint8_t code_array[NUM_DIGITS]={0,0,0,0,0};			//valid code (copy from the EEPROM), digit 0 unused
uint8_t ee_code_array[NUM_DIGITS] EEMEM={0,0,0,0,0};	//0 = not coded yet -> door only opens after recoding

int main(void)
{
	uint8_t k;

	//Inputs/Outputs
		//Outputs
	DDRA |= (1 << BIT_DOOR_1);						//PA7 as output relay door 1
	PORTA |= (1 << BIT_DOOR_1);						//PA7 HIGH -> relay not energized
	DDRA |= (1 << BIT_DOOR_2);						//PA6 as output relay door 2
	PORTA |= (1 << BIT_DOOR_2);						//PA6 HIGH -> relay not energized
	DDRB |= (1 << BIT_LED_ACKN);					//PB3 as output LED white (acknowledgement)
	PORTB &= ~(1 << BIT_LED_ACKN);					//PB3 LOW -> LED off
	DDRB |= (1 << BIT_RELAY_VCC);					//PB4 as output transistor relay supply
	PORTB &= ~(1 << BIT_RELAY_VCC);					//PB4 LOW -> relay supply off
	DDRB |= (1 << BIT_LED_CODING);					//PB5 as output LED coding
	PORTB &= ~(1 << BIT_LED_CODING);				//PB5 LOW -> LED off
		//Inputs
	DDRA &= ~(1 << BIT_CODE_BUTTON);				//PA3 as input button control panel
	PORTA |= (1 << BIT_CODE_BUTTON);				//Switch on pull-up
	DDRA &= ~(1 << BIT_JUMPER_RECODE);				//PA5 as input jumper recode
	PORTA |= (1 << BIT_JUMPER_RECODE);				//Switch on pull-up
	DDRA &= ~(1 << BIT_BUTTON_CODING);				//PA1 as input button coding (not evaluated)
	PORTA |= (1 << BIT_BUTTON_CODING);				//Switch on pull-up

	//Read code from the EEPROM
	for(k=DIGIT_CODE_1;k<NUM_DIGITS;k++) code_array[k]=eeprom_read_byte(&ee_code_array[k]);

	//Timer
	TCCR0=0b00000011;								//Timer0 prescaler 64 -> overflow every 2.048ms

	//Interrupt
	TIMSK=(1 << TOIE0);								//Timer0 overflow interrupt on
	sei();											//Switch on interrupts globally

	while(1)		//Main loop
	{
		Detect_Edge(panel_button_state, panel_button_state_before, panel_button_edge_low_high);
		Debounce_Button(PINA & (1 << BIT_CODE_BUTTON), panel_button_state, debounce_timer_panel_button, panel_button_debounce);
		Debounce_Button(PINA & (1 << BIT_JUMPER_RECODE), jumper_recode_state, debounce_timer_jumper_recode, jumper_recode_debounce);
		if(!blink_clock_finished) Blink_Clock(num_blinks, blink_clock_finished, blink_timer);

		if((jumper_recode_state)&&(sequence_operate!=OPERATE_DRIVE_OUTPUTS))		//Jumper set (only after a running relay pulse)
		{
			//Recode
			switch(sequence_coding)
			{
				case CODING_START:		PORTB |= (1 << BIT_LED_CODING);				//PB5 HIGH -> LED coding on
										sequence_operate=OPERATE_ENTRY;				//Reset operating sequence
										entry_code_ok=0;
										code_check_finished=0;
										code_entry_reset=1;							//Discard started entry
										sequence_coding=CODING_ENTRY;
										break;

				case CODING_ENTRY:		Code_Entry(panel_button_edge_low_high, entry_array, code_entry_finished, code_entry_reset, num_blinks, blink_clock_finished, entry_timer);
										if(code_entry_finished)						//All digits entered (digit 0 is ignored here)
										{
											code_entry_finished=0;
											sequence_coding=CODING_SAVE;
										}
										break;

				case CODING_SAVE:		for(k=DIGIT_CODE_1;k<NUM_DIGITS;k++)
										{
											eeprom_update_byte(&ee_code_array[k], entry_array[k]);		//Only write changed bytes
											code_array[k]=entry_array[k];							//Take over new code immediately
										}
										num_blinks=BLINK_CODE_SAVED;
										blink_clock_finished=0;						//Start blink clock
										sequence_coding=CODING_FINISHED;
										break;

				case CODING_FINISHED:	break;										//Wait until the jumper is removed
			}//end switch sequence_coding
		}
		else
		{
			if(sequence_coding!=CODING_START)			//Jumper was just removed
			{
				PORTB &= ~(1 << BIT_LED_CODING);		//PB5 LOW -> LED coding off
				code_entry_reset=1;						//Discard started entry
				sequence_coding=CODING_START;
			}

			//Operate
			switch(sequence_operate)
			{
				case OPERATE_ENTRY:			Code_Entry(panel_button_edge_low_high, entry_array, code_entry_finished, code_entry_reset, num_blinks, blink_clock_finished, entry_timer);
											if(code_entry_finished)					//All digits entered
											{
												code_entry_finished=0;
												sequence_operate=OPERATE_CHECK;
											}
											break;

				case OPERATE_CHECK:			Code_Check(entry_code_ok, code_check_finished, entry_array, code_array);
											if(entry_code_ok)						//Code correct -> drive door
											{
												entry_code_ok=0;
												code_check_finished=0;
												sequence_operate=OPERATE_DRIVE_OUTPUTS;
											}
											else
											{
												if(code_check_finished)				//Code wrong -> acknowledge, new entry
												{
													code_check_finished=0;
													num_blinks=BLINK_CODE_WRONG;
													blink_clock_finished=0;			//Start blink clock
													sequence_operate=OPERATE_ENTRY;
												}
											}
											break;

				case OPERATE_DRIVE_OUTPUTS:	Drive_Outputs(entry_array, drive_outputs_finished, timer_drive_outputs);
											if(drive_outputs_finished)				//Relay pulse finished
											{
												drive_outputs_finished=0;
												sequence_operate=OPERATE_ENTRY;
											}
											break;
			}//end switch sequence_operate
		}//end else jumper
	}//end while

}//end main

ISR(TIMER0_OVF0_vect)		//every 2.048ms (256 * 64 / 8MHz)
{
	if(debounce_timer_panel_button) debounce_timer_panel_button--;
	if(debounce_timer_jumper_recode) debounce_timer_jumper_recode--;
	if(entry_timer) entry_timer--;
	if(timer_drive_outputs) timer_drive_outputs--;
	if(blink_timer) blink_timer--;
}

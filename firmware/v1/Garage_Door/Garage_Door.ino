// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Garage_Door.ino
 *
 *  Author: Johannes Stockhammer
 *
 * Version:      1.5
 * Hardware:     Arduino Mega 2560 (ATmega2560, 16 MHz), button on pin 22,
 *               relay module (input LOW = relay energized), status LED
 * Software:     Arduino IDE, own main() with register access (no Arduino functions,
 *               Timer0 is used by the program -> no millis()/delay())
 * Description:  Garage door code lock with one button. Each digit = number of short
 *               button presses, a pause completes the digit. 1st digit = door (here
 *               only door 1), then CODE_LENGTH code digits. Code correct -> relay on for 1s.
 *               Button pressed longer than 5s -> cancel entry. After 30s without
 *               operation the relay supply is switched off.
 */

//Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "Config.h"

//Defines
	//Logic
#define TRUE 1
#define FALSE 0
	//Times (Timer0 overflow every 128µs = 256 * 8 / 16MHz)
#define TICK_US 128
#define MS_TO_TICKS(ms) ((uint32_t)(ms)*1000UL/TICK_US)
#define TIME_DEBOUNCE MS_TO_TICKS(10)			//= 78, button must be stable for 10ms
#define TIME_BLINK_CLOCK MS_TO_TICKS(100)		//= 781, half period status LED
#define TIME_PULSE MS_TO_TICKS(512)				//= 4000, pressed shorter = pulse, longer pause = digit finished
#define TIME_CANCEL MS_TO_TICKS(4992)			//= 39000, pressed longer = cancel entry
#define TIME_DRIVE_OUTPUTS MS_TO_TICKS(1000)	//= 7812, relay energized
#define TIME_INACTIVE MS_TO_TICKS(30000)		//= 234375, relay supply off afterwards
#define COUNTER_ENTRY_MAX (TIME_CANCEL+1)		//Counter stops here
#define COUNTER_INACTIVE_MAX (TIME_INACTIVE+1)	//Counter stops here
	//States
#define ENTRY 0
#define EVALUATE 1
#define DRIVE 2
#define RESET 3
	//Edges
#define EDGE_PRESSED 0
#define EDGE_RELEASED 1
#define EDGE_UNDEF 99
	//Numeric code
#define DIGIT_DOOR 0							//1st digit = door selection
#define NUM_DIGITS (1+CODE_LENGTH)				//door selection + code digits
#define NUM_DOORS 1								//V1 drives only one door
#define NUMBER_MAX 255							//no overflow to 0
	//Blinking (number of toggles of the status LED)
#define BLINK_DIGIT 2							//Blink 1x after each digit
#define BLINK_CANCEL 5							//Entry cancelled
#define BLINK_WRONG 15							//Code wrong
	//Inputs/Outputs
#define BIT_CODE_BUTTON PA0						//PA0 / pin 22
#define BIT_RELAY_DOOR_1 PA1					//PA1 / pin 23, LOW = relay energized
#define BIT_RELAY_VCC PA2						//PA2 / pin 24, HIGH = relay supply on
#define BIT_STATUS_LED PA3						//PA3 / pin 25

//Variables
	//Timers and counters (ISR, multi-byte -> only access via Timer_Read/Timer_Set)
volatile uint16_t timer_bounce=0, timer_blink=0, timer_drive_outputs=0, counter_entry=0;
volatile uint32_t counter_inactive=0;
	//Sequence
uint8_t state=ENTRY;
	//Button
bool button=FALSE, button_before=FALSE;
	//Blinking
uint8_t xblink=0;
bool xblink_clock=FALSE;
	//Entry
const uint8_t code[CODE_LENGTH]=NUMERIC_CODE;
uint8_t ecode[NUM_DIGITS];
uint8_t number=0, digit=0, edge=EDGE_UNDEF, edge_last=EDGE_UNDEF;
bool number_open=FALSE;							//TRUE = pulses counted, digit not saved yet
uint16_t time_pressed=0;
	//Inactive
bool relay_vcc_on=FALSE;

//Prototypes
void Blink_Clock();
void Debounce();
void Inactive();
void Entry();
void Evaluate();
void Drive_Outputs();
void Reset();
uint16_t Timer_Read(volatile uint16_t&timer);
uint32_t Timer_Read(volatile uint32_t&timer);
void Timer_Set(volatile uint16_t&timer, uint16_t value);
void Timer_Set(volatile uint32_t&timer, uint32_t value);

int main()
{
	//Inputs/Outputs
		//Button
	DDRA &= ~(1 << BIT_CODE_BUTTON);			//PA0 / pin 22 as input
	PORTA |= (1 << BIT_CODE_BUTTON);			//Pull-up PA0 / pin 22 activated
		//Relay door 1
	DDRA |= (1 << BIT_RELAY_DOOR_1);			//PA1 / pin 23 as output
	PORTA |= (1 << BIT_RELAY_DOOR_1);			//PA1 / pin 23 HIGH -> relay not energized
		//Relay VCC
	DDRA |= (1 << BIT_RELAY_VCC);				//PA2 / pin 24 as output
	PORTA |= (1 << BIT_RELAY_VCC);				//PA2 / pin 24 HIGH -> relay supply on
		//Status LED
	DDRA |= (1 << BIT_STATUS_LED);				//PA3 / pin 25 as output
	PORTA &= ~(1 << BIT_STATUS_LED);			//PA3 / pin 25 LOW -> status LED off

	//Timer
	TCCR0A=0b00000000;							//Normal mode, no outputs
	TCCR0B=0b00000010;							//CLK/8 prescaler -> overflow every 128µs
	TIMSK0=0b00000001;							//Activate overflow interrupt
	sei();										//Switch on interrupts globally

	while(1)		//Main loop
	{
		Blink_Clock();
		Debounce();
		Inactive();
		switch(state)
		{
			case ENTRY:		Entry();
							break;
			case EVALUATE:	Evaluate();
							break;
			case DRIVE:		Drive_Outputs();
							break;
			case RESET:		Reset();
							break;
		}//end switch
	}//end while
}//end main

//----------------------------------------------------------//
void Blink_Clock()
{
	if(!Timer_Read(timer_blink))
	{
		Timer_Set(timer_blink,TIME_BLINK_CLOCK);			//Restart timer
		if(xblink)
		{
			xblink--;
			xblink_clock=!xblink_clock;						//blink clock limited in number by xblink
			if(xblink_clock) PORTA |= (1 << BIT_STATUS_LED);	//PA3 / pin 25 HIGH -> status LED on
			else PORTA &= ~(1 << BIT_STATUS_LED);			//PA3 / pin 25 LOW -> status LED off
		}
		if((!xblink)&&(xblink_clock))						//Status LED on and blinking over
		{
			xblink_clock=FALSE;								//Start value for the next time
			PORTA &= ~(1 << BIT_STATUS_LED);				//PA3 / pin 25 LOW -> status LED off
		}
	}//end if !timer_blink
}

//----------------------------------------------------------//
void Debounce()
{
	bool button_hw=!(PINA & (1 << BIT_CODE_BUTTON));		//Pull-up: LOW = pressed

	if(button_hw==button) Timer_Set(timer_bounce,TIME_DEBOUNCE);	//State unchanged -> restart debounce time
	else if(!Timer_Read(timer_bounce)) button=button_hw;			//New state stable for 10ms -> take over
}

//----------------------------------------------------------//
void Inactive()
{
	uint32_t inactive=Timer_Read(counter_inactive);

	if((inactive<TIME_INACTIVE)&&(!relay_vcc_on))		//Operation detected
	{
		PORTA |= (1 << BIT_RELAY_VCC);					//PA2 / pin 24 HIGH -> relay supply on
		relay_vcc_on=TRUE;
	}
	if((inactive>TIME_INACTIVE)&&(relay_vcc_on))		//30s no operation
	{
		PORTA &= ~(1 << BIT_RELAY_VCC);					//PA2 / pin 24 LOW -> relay supply off
		state=RESET;									//Discard started entry
		relay_vcc_on=FALSE;
	}
}

//----------------------------------------------------------//
void Entry()
{
	uint16_t time;

	//Detect edge
	if((button)&&(!button_before))						//Button pressed
	{
		edge=EDGE_PRESSED;
		edge_last=EDGE_PRESSED;
	}
	else
	{
		if((!button)&&(button_before))					//Button released
		{
			edge=EDGE_RELEASED;
			edge_last=EDGE_RELEASED;
		}
	}
	button_before=button;

	//Once per edge
	switch(edge)
	{
		case EDGE_PRESSED:			Timer_Set(counter_entry,0);				//Measure press duration
									Timer_Set(counter_inactive,0);			//Operation detected
									edge=EDGE_UNDEF;
									break;

		case EDGE_RELEASED:			Timer_Set(counter_entry,0);				//Measure pause duration
									Timer_Set(counter_inactive,0);
									if(time_pressed>TIME_CANCEL)			//Pressed very long -> cancel
									{
										state=RESET;
										xblink=BLINK_CANCEL;
									}
									if(time_pressed<TIME_PULSE)				//Short press = one pulse
									{
										if(number<NUMBER_MAX) number++;
										number_open=TRUE;
									}
									else
									{
										number_open=FALSE;					//Long press does not count
									}
									edge=EDGE_UNDEF;
									break;
	}//end switch edge

	//Continuously depending on the last edge
	switch(edge_last)
	{
		case EDGE_PRESSED:			time_pressed=Timer_Read(counter_entry);			//Record press duration
									break;

		case EDGE_RELEASED:			time=Timer_Read(counter_entry);					//Pause duration
									if((time>TIME_PULSE)&&(number_open))			//Pause long enough -> digit finished
									{
										ecode[digit]=number;
										number=0;
										number_open=FALSE;
										if(digit<(NUM_DIGITS-1)) xblink=BLINK_DIGIT;	//Acknowledge digit (not the last one)
										digit++;
										if(digit>=NUM_DIGITS)						//All digits entered
										{
											digit=0;
											state=EVALUATE;
										}
									}
									break;
	}//end switch edge_last
}//end Entry

//----------------------------------------------------------//
void Evaluate()
{
	uint8_t k;
	bool code_ok=TRUE;											//Start value: code correct

	if((ecode[DIGIT_DOOR]<1)||(ecode[DIGIT_DOOR]>NUM_DOORS)) code_ok=FALSE;	//Check door selection
	for(k=0;k<CODE_LENGTH;k++)
	{
		if(ecode[k+1]!=code[k]) code_ok=FALSE;					//Digit wrong
	}

	if(code_ok)
	{
		state=DRIVE;
		Timer_Set(timer_drive_outputs,TIME_DRIVE_OUTPUTS);		//Start relay time
	}
	else
	{
		state=RESET;
		xblink=BLINK_WRONG;
	}
}

//----------------------------------------------------------//
void Drive_Outputs()
{
	if(Timer_Read(timer_drive_outputs))						//Time running
	{
		PORTA &= ~(1 << BIT_RELAY_DOOR_1);					//PA1 / pin 23 LOW -> relay energized
	}
	else													//Time expired
	{
		PORTA |= (1 << BIT_RELAY_DOOR_1);					//PA1 / pin 23 HIGH -> relay not energized
		state=RESET;
	}
}

//----------------------------------------------------------//
void Reset()
{
	digit=0;
	number=0;
	number_open=FALSE;
	edge=EDGE_UNDEF;
	edge_last=EDGE_UNDEF;
	time_pressed=0;
	Timer_Set(counter_entry,0);
	Timer_Set(timer_drive_outputs,0);
	state=ENTRY;
}

//----------------------------------------------------------//
//Atomic access to the multi-byte ISR variables
uint16_t Timer_Read(volatile uint16_t&timer)
{
	uint16_t value;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		value=timer;
	}
	return value;
}

uint32_t Timer_Read(volatile uint32_t&timer)
{
	uint32_t value;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		value=timer;
	}
	return value;
}

void Timer_Set(volatile uint16_t&timer, uint16_t value)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		timer=value;
	}
}

void Timer_Set(volatile uint32_t&timer, uint32_t value)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		timer=value;
	}
}

//----------------------------------------------------------//
ISR(TIMER0_OVF_vect)		//every 128µs
{
	if(timer_bounce) timer_bounce--;
	if(counter_entry<COUNTER_ENTRY_MAX) counter_entry++;
	if(counter_inactive<COUNTER_INACTIVE_MAX) counter_inactive++;
	if(timer_drive_outputs) timer_drive_outputs--;
	if(timer_blink) timer_blink--;
}

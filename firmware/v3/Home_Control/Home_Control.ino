// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Home_Control.ino
 *
 *  Author: Johannes Stockhammer
 *
 * Version:      3.0
 * Hardware:     Arduino Mega 2560 (ATmega2560, 16 MHz), button on pin 22, LED on pin 39,
 *               relay supply pin 30, relay door 1 pin 31, relay door 2 pin 32
 * Software:     Arduino IDE, own main() with register access (no Arduino functions,
 *               Timer0 is used by the program -> no millis()/delay())
 * Description:  Garage door control as the first module of a home control.
 *               Code is entered with one button (number of button presses per digit),
 *               1st digit = door 1 or 2, then CODE_LENGTH code digits.
 */

//Includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "Config.h"

//Defines
	//Times (Timer0 overflow every 1.024ms = 256 * 64 / 16MHz)
#define TICK_US 1024
#define MS_TO_TICKS(ms) ((((uint32_t)(ms)*1000UL)+(TICK_US/2))/TICK_US)	//Convert ms to ISR runs (rounded)
#define TIME_10ms MS_TO_TICKS(10)			//= 10
#define TIME_100ms MS_TO_TICKS(100)			//= 98
#define TIME_1sec MS_TO_TICKS(1000)			//= 977
	//Read_Buttons
#define EDGE_FALLING 0						//Button pressed (pull-up -> HIGH to LOW)
#define EDGE_RISING 1						//Button released
#define EDGE_UNDEF 3
	//Garage_Door_Control
#define DIGIT_DOOR 0						//1st digit = door selection
#define DIGIT_MAX (1+CODE_LENGTH)			//door selection + code digits
#define NUM_DOORS 2							//door 1 and door 2
#define NUMBER_MAX 255						//no overflow to 0
	//Inputs/Outputs
#define BIT_CODE_BUTTON 0					//PA0 or digital pin 22
#define BIT_LED 2							//PG2 or digital pin 39
#define BIT_RELAY_VCC 7						//PC7 or digital pin 30, HIGH = relay supply on
#define BIT_RELAY_DOOR_1 6					//PC6 or digital pin 31, LOW = relay energized
#define BIT_RELAY_DOOR_2 5					//PC5 or digital pin 32, LOW = relay energized

//Prototypes
void Input_Init();
void Timer_Init();
void Garage_Door_Control(uint8_t&button_1_edge, const uint8_t numeric_code[CODE_LENGTH]);
void Read_Buttons(uint8_t&button_1_edge);
uint16_t Timer_Read(volatile uint16_t&timer);
void Timer_Set(volatile uint16_t&timer, uint16_t value);

//Timer variables (counted down in the ISR)
volatile uint8_t timer_button_1=0;			//8 bit -> access atomic
volatile uint8_t timer_blink_clock=0;		//8 bit -> access atomic
volatile uint16_t entry_timer=0;			//16 bit -> only via Timer_Read/Timer_Set

int main()
{
	//Variables Read_Buttons
	uint8_t button_1_edge=EDGE_UNDEF;
	//Numeric code (without door selection)
	const uint8_t numeric_code[CODE_LENGTH]=NUMERIC_CODE;

	Input_Init();
	Timer_Init();

	while(1)		//Main loop
	{
		Read_Buttons(button_1_edge);
		Garage_Door_Control(button_1_edge,numeric_code);
	}//end while
}//end main

ISR(TIMER0_OVF_vect)	//Interrupt every 1.024ms
{
	if(timer_button_1) timer_button_1--;
	if(entry_timer) entry_timer--;
	if(timer_blink_clock) timer_blink_clock--;
}

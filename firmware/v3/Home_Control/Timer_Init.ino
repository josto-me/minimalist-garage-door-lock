// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Timer_Init.ino
 *
 *  Author: Johannes Stockhammer
 */

void Timer_Init()
{
	//8-bit Timer/Counter 0
	TCCR0A &= ~( (1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 1) | (1 << 0) );		//No outputs, normal mode
	TCCR0B &= ~( (1 << 7) | (1 << 6) | (1 << 3) | (1 << 2) );							//No force output compare, WGM02=0
	TCCR0B |= (1 << 1) | (1 << 0);														//CLK/64 prescaler -> overflow every 1.024ms
	TIMSK0 &= ~( (1 << 2) | (1 << 1) );													//No compare match interrupts
	TIMSK0 |= (1 << 0);																	//Timer overflow interrupt on

	//Switch on interrupts globally
	sei();
}

// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Blink_Clock.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Lets the acknowledge LED blink num_blinks times (num_blinks*2 toggles)
 * and sets blink_clock_finished afterwards.
 */
#include "Blink_Clock.h"

void Blink_Clock(uint8_t num_blinks, bool&blink_clock_finished, volatile uint8_t&blink_timer)
{
	static uint8_t x=0;												//Number of toggles

	if(!blink_clock_finished)
	{
		if(!blink_timer)
		{
			blink_timer=TIME_BLINK_CLOCK;							//Restart timer
			PORTB ^= (1 << BIT_LED_ACKN);							//Invert PB3 (let LED blink)
			x++;
		}

		if((uint16_t)num_blinks*2<=x)								//All blinks output
		{
			blink_clock_finished=1;
			PORTB &= ~(1 << BIT_LED_ACKN);							//PB3 LOW -> LED off
			x=0;
		}
	}
}

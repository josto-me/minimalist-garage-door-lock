// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Debounce_Button.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Debounces a button or jumper to GND (pull-up: pin LOW = ON).
 * A new state is only taken over once it has been stable for TIME_DEBOUNCE.
 * The flag debounce_active belongs to the respective button and is therefore passed
 * by reference.
 */
#include "Debounce_Button.h"

void Debounce_Button(bool button_hw_state, bool&button_sw_state, volatile uint8_t&debounce_timer, bool&debounce_active)
{
	bool button_pressed=!button_hw_state;			//Pull-up: LOW = pressed or jumper set

	if(button_pressed!=button_sw_state)				//Pin differs from the debounced state
	{
		if(!debounce_active)
		{
			debounce_timer=TIME_DEBOUNCE;			//Start debounce time
			debounce_active=1;
		}
		else
		{
			if(!debounce_timer)						//Time expired and pin still different -> save state after bouncing
			{
				button_sw_state=button_pressed;		//ON = pressed, OFF = released
				debounce_active=0;
			}
		}
	}
	else
	{
		debounce_active=0;							//Pin back as before (bouncing) -> cancel debouncing
	}
}

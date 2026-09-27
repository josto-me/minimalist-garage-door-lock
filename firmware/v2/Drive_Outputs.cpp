// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Drive_Outputs.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Switches the relay supply on and energizes the relay of the selected door
 * for TIME_DRIVE_OUTPUTS (pulse to the door opener).
 */
#include "Drive_Outputs.h"

void Drive_Outputs(const uint8_t entry_array[NUM_DIGITS], bool&drive_outputs_finished, volatile uint8_t&timer_drive_outputs)
{
	static uint8_t case_drive_outputs=DRIVE_START;

	switch(case_drive_outputs)
	{
		case DRIVE_START:		if(!drive_outputs_finished)
								{
									PORTB |= (1 << BIT_RELAY_VCC);				//PB4 HIGH -> transistor conducts, relay supply on
									switch(entry_array[DIGIT_DOOR])
									{
										case 1:	PORTA &= ~(1 << BIT_DOOR_1);	//PA7 LOW -> relay door 1 energized
												break;
										case 2:	PORTA &= ~(1 << BIT_DOOR_2);	//PA6 LOW -> relay door 2 energized
												break;
									}
									timer_drive_outputs=TIME_DRIVE_OUTPUTS;		//Start pulse time
									case_drive_outputs=DRIVE_WAIT;
								}
								break;

		case DRIVE_WAIT:		if(!timer_drive_outputs)						//Pulse time expired
								{
									PORTA |= (1 << BIT_DOOR_1);					//PA7 HIGH -> relay door 1 released
									PORTA |= (1 << BIT_DOOR_2);					//PA6 HIGH -> relay door 2 released
									PORTB &= ~(1 << BIT_RELAY_VCC);				//PB4 LOW -> relay supply off
									drive_outputs_finished=1;
									case_drive_outputs=DRIVE_START;
								}
								break;
	}//end switch
}

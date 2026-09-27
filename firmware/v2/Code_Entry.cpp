// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Code_Entry.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Counts the button presses per digit. After TIME_NEXT_DIGIT without another press
 * the number is saved and the next digit begins. After NUM_DIGITS digits
 * the entry is finished (code_entry_finished).
 */
#include "Code_Entry.h"

void Code_Entry(bool&edge, uint8_t entry_array[NUM_DIGITS], bool&code_entry_finished, bool&code_entry_reset, uint8_t&num_blink_clock, bool&blink_clock_finished, volatile uint8_t&entry_timer)
{
	static uint8_t digit=0, number=0, case_entry=ENTRY_COUNT;

	switch(case_entry)
	{
		case ENTRY_COUNT:		if(edge)								//If edge detected (button pressed)
								{
									edge=0;								//Edge consumed
									entry_timer=TIME_NEXT_DIGIT;		//Restart time until the next digit
									if(number<NUMBER_MAX) number++;		//Count pulse (no overflow to 0)
								}
								if((!entry_timer)&&(number))			//Pause expired and at least one pulse
								{
									case_entry=ENTRY_SAVE;
									if(digit<(NUM_DIGITS-1))			//Acknowledge after each digit except the last one
									{
										num_blink_clock=BLINK_DIGIT;
										blink_clock_finished=0;			//Start blink clock
									}
								}
								if(code_entry_reset)					//Discard entry from outside
								{
									code_entry_reset=0;
									case_entry=ENTRY_COUNT;
									edge=0;
									number=0;
									digit=0;
									code_entry_finished=0;
									entry_timer=0;
								}
								break;

		case ENTRY_SAVE:		entry_array[digit]=number;				//Save number of the current digit
								number=0;
								case_entry=ENTRY_COUNT;
								if(digit>=(NUM_DIGITS-1))				//Last digit -> entry finished
								{
									digit=0;
									code_entry_finished=1;
								}
								else
								{
									digit++;							//Next digit
								}
								break;
	}//end switch
}

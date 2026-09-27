// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Code_Check.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Compares the entry with the code from the EEPROM.
 * Digit 0 is the door selection (1..NUM_DOORS), digits 1..4 are the actual code.
 * An EEPROM that is not coded (0 or 0xFF) is never accepted as correct.
 */
#include "Code_Check.h"

void Code_Check(bool&code_ok, bool&code_check_finished, const uint8_t entry_array[NUM_DIGITS], const uint8_t code_array[NUM_DIGITS])
{
	uint8_t k;
	bool code_ok_=1;																		//Start value: code correct

	if((entry_array[DIGIT_DOOR]<1)||(entry_array[DIGIT_DOOR]>NUM_DOORS)) code_ok_=0;		//Door selection only 1 or 2

	for(k=DIGIT_CODE_1;k<NUM_DIGITS;k++)
	{
		if((code_array[k]==CODE_EMPTY_0)||(code_array[k]==CODE_EMPTY_FF)) code_ok_=0;	//Not coded yet -> never open
		if(entry_array[k]!=code_array[k]) code_ok_=0;									//Digit wrong
	}

	code_ok=code_ok_;
	code_check_finished=1;
}

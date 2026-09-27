// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Garage_Door_Control.ino
 *
 *  Author: Johannes Stockhammer
 *
 * Code entry with one button: each release within TIME_1sec counts the
 * current digit up by one. After TIME_1sec without a release the digit is
 * saved. After DIGIT_MAX digits the code is checked and the relay of the door is driven.
 */

//Defines
	//Sequence
#define ENTRY_START 0
#define ENTRY_NUMBERS 1
#define CODE_CHECK 2
#define DRIVE_DOOR 3
#define RELAY_OFF 4
	//Blinking (number of toggles of the LED)
#define BLINK_DIGIT 2							//Blink 1x after each digit
#define BLINK_WRONG 6							//Blink 3x on wrong code

void Garage_Door_Control(uint8_t&button_1_edge, const uint8_t numeric_code[CODE_LENGTH])
{
	static uint8_t case_entry=ENTRY_START, entry_array[DIGIT_MAX], digit=0, number=0, num_blink_clock=0;

	//Blink clock
	if( (!timer_blink_clock) && num_blink_clock )
	{
		timer_blink_clock=TIME_100ms;					//Set timer again
		PORTG ^= (1 << BIT_LED);						//Invert PG2 or digital pin 39 (let LED blink)
		num_blink_clock--;								//Count number down
		if(!num_blink_clock) PORTG &= ~(1 << BIT_LED);	//Finished -> LED safely off
	}

	//Code entry
	switch(case_entry)
	{
		//When the button is pressed (code entry begins)
		case ENTRY_START:	if(button_1_edge==EDGE_FALLING)					//If falling edge on the button (pressed)
							{
								button_1_edge=EDGE_UNDEF;					//Reset to UNDEF for the next detection
								Timer_Set(entry_timer,TIME_1sec);			//Time within which the button must be released
								case_entry=ENTRY_NUMBERS;					//Jump to the next case
							}
							break;

		//Enter and save code
		case ENTRY_NUMBERS:	if( Timer_Read(entry_timer) && (button_1_edge==EDGE_RISING) )			//If button released and time not expired yet
							{
								button_1_edge=EDGE_UNDEF;					//Reset edge to UNDEF for the next detection
								Timer_Set(entry_timer,TIME_1sec);			//Restart timer until the next digit
								if(number<NUMBER_MAX) number++;				//Count number up by one
							}
							else
							{
								if(!Timer_Read(entry_timer))				//If entry timer expired (next digit)
								{
									Timer_Set(entry_timer,TIME_1sec);		//Set timer again
									entry_array[digit]=number;				//Save current number value
									number=0;								//Reset number for the next entry
									if(digit<(DIGIT_MAX-1))
									{
										digit++;							//Next digit
										num_blink_clock=BLINK_DIGIT;		//Acknowledge digit
									}
									else
									{
										digit=0;							//Reset digit for the next entry
										case_entry=CODE_CHECK;				//Entry finished -> check code
									}
								}
							}
							break;

		//Check code
		case CODE_CHECK:	case_entry=DRIVE_DOOR;												//Start value: code correct (reset if a digit is wrong)
							if( (entry_array[DIGIT_DOOR]==0) || (entry_array[DIGIT_DOOR]>NUM_DOORS) )	//Door selection only 1 or 2
							{
								case_entry=ENTRY_START;
								num_blink_clock=BLINK_WRONG;
							}
							else
							{
								for(digit=0;digit<CODE_LENGTH;digit++)							//Check code
								{
									if( numeric_code[digit]!=entry_array[digit+1] )				//If digit wrong
									{
										case_entry=ENTRY_START;									//Start new entry
										num_blink_clock=BLINK_WRONG;
									}
								}
								digit=0;														//Reset digit to 0 again
							}
							break;

		//Drive door
		case DRIVE_DOOR:	PORTC |= (1 << BIT_RELAY_VCC);								//PC7 or pin 30 HIGH (supply voltage of the relays on)
							if(entry_array[DIGIT_DOOR]==1) PORTC &= ~(1 << BIT_RELAY_DOOR_1);	//PC6 or pin 31 LOW (relay door 1 energized)
							else PORTC &= ~(1 << BIT_RELAY_DOOR_2);							//PC5 or pin 32 LOW (relay door 2 energized)
							Timer_Set(entry_timer,TIME_1sec);								//Start relay time
							case_entry=RELAY_OFF;											//Jump to the next case
							break;

		//Switch relay off
		case RELAY_OFF:		if(!Timer_Read(entry_timer))
							{
								PORTC &= ~(1 << BIT_RELAY_VCC);				//PC7 or pin 30 LOW (supply voltage of the relays off)
								PORTC |= (1 << BIT_RELAY_DOOR_1);			//PC6 or pin 31 HIGH (relay door 1 not energized)
								PORTC |= (1 << BIT_RELAY_DOOR_2);			//PC5 or pin 32 HIGH (relay door 2 not energized)
								case_entry=ENTRY_START;						//Ready again for a new code entry
							}
							break;
	}//end switch
}

// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Read_Buttons.ino
 *
 *  Author: Johannes Stockhammer
 *
 * Debounces the code button (state must be stable for TIME_10ms) and reports
 * the edges via button_1_edge. The consumer resets it to EDGE_UNDEF.
 */

void Read_Buttons(uint8_t&button_1_edge)
{
	//Button 1 - debounce
	static uint8_t case_button_1=1;						//Variable for switch/case debounce button 1 (default HIGH, because of pull-up)
	static bool button_1_state=1;						//Stores the state of the debounced button (default HIGH, because of pull-up)
	//Button 1 - detect edge
	static bool button_1_state_before=1;				//Also HIGH, otherwise edge detection at power-on

	//Debounce button 1
	switch(case_button_1)
	{
		//Candidate state LOW
		case 0:	if( timer_button_1 && (PINA & (1 << BIT_CODE_BUTTON)) )		//If timer not expired and input HIGH
				{
					timer_button_1=TIME_10ms;
					case_button_1=1;										//Candidate HIGH -> jump to the other case and check
				}
				else
				{
					if( !timer_button_1 )									//Timer expired (state unchanged during the last period)
					{
						button_1_state=0;									//Save button state
						timer_button_1=TIME_10ms;
					}
				}
				break;

		//Candidate state HIGH
		case 1:	if( timer_button_1 && (!(PINA & (1 << BIT_CODE_BUTTON))) )	//If timer not expired and input LOW
				{
					timer_button_1=TIME_10ms;
					case_button_1=0;										//Candidate LOW -> jump to the other case and check
				}
				else
				{
					if( !timer_button_1 )									//Timer expired (state unchanged during the last period)
					{
						button_1_state=1;									//Save button state
						timer_button_1=TIME_10ms;
					}
				}
				break;
	}//end switch

	//Button 1 detect edge
	if( (!button_1_state_before) && (button_1_state) ) button_1_edge=EDGE_RISING;				//Edge rising = released
	else if( (button_1_state_before) && (!button_1_state) ) button_1_edge=EDGE_FALLING;		//Edge falling = pressed
	button_1_state_before=button_1_state;														//Save last button state
}

// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Input_Init.ino
 *
 *  Author: Johannes Stockhammer
 */

void Input_Init()
{
	//Input_Code_Button
	DDRA &= ~(1 << BIT_CODE_BUTTON);	//PA0 or digital pin 22 as input
	PORTA |= (1 << BIT_CODE_BUTTON);	//Pull-up on PA0 or digital pin 22 active
	//Output_LED (blink clock)
	DDRG |= (1 << BIT_LED);				//PG2 or digital pin 39 as output
	PORTG &= ~(1 << BIT_LED);			//Set PG2 or digital pin 39 to LOW (LED off)
	//Output_Relay_Supply_Voltage
	DDRC |= (1 << BIT_RELAY_VCC);		//PC7 or digital pin 30 as output
	PORTC &= ~(1 << BIT_RELAY_VCC);		//Set PC7 or digital pin 30 to LOW (relay supply off)
	//Output_Relay_Control_Signal_Door_1
	DDRC |= (1 << BIT_RELAY_DOOR_1);	//PC6 or digital pin 31 as output
	PORTC |= (1 << BIT_RELAY_DOOR_1);	//Set PC6 or digital pin 31 to HIGH (relay not energized)
	//Output_Relay_Control_Signal_Door_2
	DDRC |= (1 << BIT_RELAY_DOOR_2);	//PC5 or digital pin 32 as output
	PORTC |= (1 << BIT_RELAY_DOOR_2);	//Set PC5 or digital pin 32 to HIGH (relay not energized)
}

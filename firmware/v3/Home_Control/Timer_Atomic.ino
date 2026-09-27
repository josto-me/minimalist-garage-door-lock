// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Timer_Atomic.ino
 *
 *  Author: Johannes Stockhammer
 *
 * 16-bit timers that are counted down in the ISR may only be read and written
 * with interrupts disabled on the AVR (2 byte access).
 */

uint16_t Timer_Read(volatile uint16_t&timer)
{
	uint16_t value;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		value=timer;					//Read both bytes without ISR in between
	}
	return value;
}

void Timer_Set(volatile uint16_t&timer, uint16_t value)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		timer=value;					//Write both bytes without ISR in between
	}
}

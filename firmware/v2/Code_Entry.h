// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Code_Entry.h
 *
 *  Author: Johannes Stockhammer
 */


#ifndef CODE_ENTRY_H_
#define CODE_ENTRY_H_

#include "Config.h"
#define TIME_NEXT_DIGIT MS_TO_TICKS(522)		//522ms / 2.048ms = 255 ISR runs pause -> next digit
#define BLINK_DIGIT 1							//Blink 1x after each digit (except the last one)
//Sequence Code_Entry
#define ENTRY_COUNT 0
#define ENTRY_SAVE 1
void Code_Entry(bool&edge, uint8_t entry_array[NUM_DIGITS], bool&code_entry_finished, bool&code_entry_reset, uint8_t&num_blink_clock, bool&blink_clock_finished, volatile uint8_t&entry_timer);

#endif /* CODE_ENTRY_H_ */

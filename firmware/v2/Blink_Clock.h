// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Blink_Clock.h
 *
 *  Author: Johannes Stockhammer
 */


#ifndef BLINK_CLOCK_H_
#define BLINK_CLOCK_H_

#include "Config.h"
#define TIME_BLINK_CLOCK MS_TO_TICKS(102)	//102ms / 2.048ms = 50 ISR runs per half period
void Blink_Clock(uint8_t num_blinks, bool&blink_clock_finished, volatile uint8_t&blink_timer);

#endif /* BLINK_CLOCK_H_ */

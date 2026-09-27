// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Debounce_Button.h
 *
 *  Author: Johannes Stockhammer
 */


#ifndef DEBOUNCE_BUTTON_H_
#define DEBOUNCE_BUTTON_H_

#include "Config.h"
#define TIME_DEBOUNCE MS_TO_TICKS(10)		//10ms / 2.048ms = 5 ISR runs
void Debounce_Button(bool button_hw_state, bool&button_sw_state, volatile uint8_t&debounce_timer, bool&debounce_active);

#endif /* DEBOUNCE_BUTTON_H_ */

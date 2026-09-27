// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Detect_Edge.cpp
 *
 *  Author: Johannes Stockhammer
 *
 * Detects the change OFF -> ON (button is pressed) on the debounced button state.
 * edge_low_high stays set until the consumer (Code_Entry) resets it.
 */
#include "Detect_Edge.h"

void Detect_Edge(bool button_state, bool&button_state_before, bool&edge_low_high)
{
	if((!button_state_before)&&(button_state)) edge_low_high=1;			//OFF -> ON = button pressed
	button_state_before=button_state;									//Remember state for the next run
}

// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Drive_Outputs.h
 *
 *  Author: Johannes Stockhammer
 */


#ifndef DRIVE_OUTPUTS_H_
#define DRIVE_OUTPUTS_H_

#include "Config.h"
#define TIME_DRIVE_OUTPUTS MS_TO_TICKS(522)	//522ms / 2.048ms = 255 ISR runs relay energized
//Sequence Drive_Outputs
#define DRIVE_START 0
#define DRIVE_WAIT 1
void Drive_Outputs(const uint8_t entry_array[NUM_DIGITS], bool&drive_outputs_finished, volatile uint8_t&timer_drive_outputs);

#endif /* DRIVE_OUTPUTS_H_ */

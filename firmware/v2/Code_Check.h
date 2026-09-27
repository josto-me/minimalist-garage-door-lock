// SPDX-FileCopyrightText: Johannes Stockhammer
// SPDX-License-Identifier: Apache-2.0
/*
 * Code_Check.h
 *
 *  Author: Johannes Stockhammer
 */


#ifndef CODE_CHECK_H_
#define CODE_CHECK_H_

#include "Config.h"
void Code_Check(bool&code_ok, bool&code_check_finished, const uint8_t entry_array[NUM_DIGITS], const uint8_t code_array[NUM_DIGITS]);

#endif /* CODE_CHECK_H_ */

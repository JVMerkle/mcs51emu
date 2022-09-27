/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "mcs51.h"

void mcs51_reset_and_load_instruction_register(mcs51_t* p, opcode_t opcode);

void mcs51_load_instruction_register_arguments(mcs51_t* p, uint8_t arg1, uint8_t arg2, uint8_t arg3);

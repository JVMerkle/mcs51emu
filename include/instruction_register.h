/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "opcode.h"
#include <stdbool.h>

typedef struct instruction_register_t {
    opcode_t opcode;
    uint8_t args[3];

    bool accessed_sfr_ie;
    bool accessed_sfr_ip;
} instruction_register_t;

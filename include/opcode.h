/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

typedef struct mcs51_t mcs51_t;

typedef struct opcode_t {
    uint8_t code;
    uint8_t bytes;
    uint8_t cycles;

    const char* mnemonic;
    const char* arg1;
    const char* arg2;
    const char* arg3;

    void (*actor)(mcs51_t*);
} opcode_t;

void opcode_print(opcode_t* opcode);

/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#include "opcode.h"
#include <stdio.h>

void opcode_print(opcode_t* opcode)
{
    printf("%s %s %s %s",
           opcode->mnemonic, opcode->arg1, opcode->arg2, opcode->arg3);
}

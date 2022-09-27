/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

typedef struct mcs51_t mcs51_t;

void mcs51_register_opcodes(mcs51_t* p);
void mcs51_register_sfrs(mcs51_t* p);

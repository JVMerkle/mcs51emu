/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct mcs51_t mcs51_t;

typedef struct sfr_t {
    uint8_t address;
    const char* name;
    bool bit_addressable;

    void (*on_read)(struct sfr_t*, mcs51_t*);
    void (*on_write)(struct sfr_t*, mcs51_t*);
} sfr_t;

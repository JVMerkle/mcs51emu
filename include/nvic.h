/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct mcs51_t mcs51_t;

typedef struct interrupt_t {
    const char* name;
    uint8_t bit_mask;

    uint16_t vector;

    uint8_t sfr_address;
    uint8_t sfr_bit_mask;
    bool clears_flag;
} interrupt_t;

/**
 * Interrupts:
 * | Name    | Flag  | Address
 * | Reset   | -     | 0x0000
 * | INT0    | IE0   | 0x0003
 * | Timer 0 | TF0   | 0x000B
 * | INT1    | IE1   | 0x0013
 * | Timer 1 | TF1   | 0x001B
 * | Serial  | TI/RI | 0x0023
 */
typedef struct nvic_t {
    // SFR_IE map
    interrupt_t map[5];

    /**
     * Pending ISRs (bitmask). Compatible to SFR_IE and SFR_IP.
     * MSB [ RI/TI | TF1 | IE1 | TF0 | IE0 ] LSB
     */
    uint8_t _isr_pending;

    uint8_t _isr_active_msk;  /// Active ISRs (bitmask)
    uint8_t _isr_running_msk; /// Currently active and running ISR (bit mask)

    uint8_t _ljmp_vector;
} nvic_t;

void nvic_init(nvic_t* nvic);

void nvic_reset(nvic_t* nvic);

void nvic_reti(nvic_t* nvic, mcs51_t* p);

void nvic_latch_interrupt_flags(nvic_t* nvic, mcs51_t* p);

void nvic_run_interrupt_controller(nvic_t* nvic, mcs51_t* p);

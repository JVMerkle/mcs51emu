/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "opcode.h"
#include <stdbool.h>
#include <stdint.h>

#include "instruction_register.h"
#include "nvic.h"
#include "sfr.h"

/**
 * Intel MCS-51 MCU (aka. 8051).
 *
 * DATA       D:00 – D:7F 	Direct addressable on chip RAM.
 * BIT        D:20 – D:2F 	bit addressable RAM; accessed bit instructions.
 * IDATA      I:00 – I:FF 	Indirect addressable on chip RAM; can be accessed with @R0 or @R1.
 * XDATA      X:0000 – X:FFFF 	64 KB RAM (read/write access). Accessed with MOVX instruction.
 * CODE       C:0000 – C:FFFF 	64 KB ROM (only read access possible). Used for executable code or constants.
 * BANK 0
 * ...
 * BANK 31    B0:0000 – B0:FFFF
 * B31:0000 – B31:FFFF  Code Banks for expanding the program code space to 32 x 64KB ROM.
 *
 * Facts:
 * - Oscillator 11.0592 MHz when C/T bit of TMOD is 0
 * - 1 machine cycle = 12 clock cycles
 * - UART circuit clock divider = 32
 *
 * https://ww1.microchip.com/downloads/en/DeviceDoc/doc4316.pdf
 * F_PER = F_OSC / 2 in Standard mode (X1)
 * F_PER = F_OSC / 1 in X2
 * 1 Peripheral cycle = F_PER / 6
 * F_TIM = F_PER / 6
 *
 * Indirect addresses below 0x80 are mapped into the lower DATA region ("physically" 0x00-07F)
 * Indirect addresses above 0x80 are mapped into the upper DATA region ("physically" 0x100-0x17F)
 * The SFR region 0x80-0xFF is inaccessibly via indirect addressing mode.
 */
typedef struct mcs51_t {
    uint16_t PC; /// Program counter, the only register that is not mmapped in the 8051.

    uint8_t D[0x200];   /// 128 DATA, 128 SFRs, 128 IDATA
    uint8_t X[0x10000]; /// XDATA
    uint8_t C[0x10000]; /// CODE

    sfr_t sfr_map[0x100]; /// Describes and handles directly addressable memory (such as R0, R1, ..., SFRs)
    opcode_t opcode_map[0x100];

    uint64_t _osc_frequency_hertz;
    uint64_t _osc_periods;

    instruction_register_t _instruction_register;

    /**
     * The main function of ALE is to provide a properly timed signal to latch the low byte of an
     * address from P0 to an external latch during fetches from external Program Memory. For
     * that purpose ALE is activated twice every machine cycle. This activation takes place
     * even when the cycle involves no external fetch. The only time an ALE pulse doesn’t
     * come out is during an access to external Data Memory. The first ALE of the second
     * cycle of a MOVX instructions is missing. The ALE disable mode, described in Section
     * 2.8.2, disables the ALE output. Consequently, in any system that does not use external
     * Data Memory, ALE is activated at a constant rate of 1/6 the oscillator frequency, and
     * can be used for external clocking or timing purposes.
     */
    bool _ale; /// Address Latch Enable

    void (*_state_phases[12])(mcs51_t*);

    nvic_t _nvic;

    bool _sfr_dirty_sbuf;

    void (*_on_serial_tx)(char c);
    bool _abort_on_unimplemented_opcode;
} mcs51_t;

void mcs51_init(mcs51_t* p);

void mcs51_reset(mcs51_t* p);

void mcs51_print_state(mcs51_t* p);

void mcs51_print_current_instruction(mcs51_t* p);

double msc51_execution_time_ms(mcs51_t* p);

void msc51_do_machine_cycle(mcs51_t* p);

void msc51_do_osc_period(mcs51_t* p);

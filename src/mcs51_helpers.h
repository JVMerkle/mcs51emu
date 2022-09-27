/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "sfr_definitions_gen.h"
#include <assert.h>
#include <stdio.h>

static inline uint8_t register_bank_index(mcs51_t* p)
{
    return p->D[SFR_PSW] >> 3 & 0b11;
}

#define GET_C()   ((p->D[SFR_PSW] >> 7) & 0b1)
#define SET_C()   p->D[SFR_PSW] |= (1 << 7)
#define CLEAR_C() p->D[SFR_PSW] &= ~(1 << 7)

#define SP        (p->D[SFR_SP])
#define ACC       (p->D[SFR_ACC])
#define R0        (p->D[register_bank_index(p) * 0x8 + 0x0])
#define R1        (p->D[register_bank_index(p) * 0x8 + 0x1])
#define R2        (p->D[register_bank_index(p) * 0x8 + 0x2])
#define R3        (p->D[register_bank_index(p) * 0x8 + 0x3])
#define R4        (p->D[register_bank_index(p) * 0x8 + 0x4])
#define R5        (p->D[register_bank_index(p) * 0x8 + 0x5])
#define R6        (p->D[register_bank_index(p) * 0x8 + 0x6])
#define R7        (p->D[register_bank_index(p) * 0x8 + 0x7])

/**
 * Translate an address in indirect addressing mode to a "physical" address.
 * The SFR region is inaccessible via indirect addressing mode.
 */
static inline uint16_t to_indirect_address(uint8_t address)
{
    uint16_t indirect = address;

    // Select bit 7 and shift it left: 0x80 -> 0x100
    uint16_t bit_7 = indirect & 0x80;
    // Clear bit 7
    indirect &= ~bit_7;
    // Apply bit 8
    indirect |= bit_7 << 1;

    return indirect;
}


static inline void check_sfr_write_access(mcs51_t* p, uint8_t address)
{
    sfr_t* sfr = &p->sfr_map[address];
    sfr->on_write(sfr, p);
}

static inline void check_sfr_read_access(mcs51_t* p, uint8_t address)
{
    sfr_t* sfr = &p->sfr_map[address];
    sfr->on_read(sfr, p);
}

static inline uint8_t pop_pc_u8(mcs51_t* p)
{
    return p->C[p->PC++]; // Post-increment
}

static inline int8_t pop_pc_s8(mcs51_t* p)
{
    return (int8_t) pop_pc_u8(p);
}

static inline uint16_t pop_pc_u16(mcs51_t* p)
{
    return (((uint16_t) pop_pc_u8(p)) << 8) | pop_pc_u8(p);
}

static inline void push_sp_u8(mcs51_t* p, uint8_t v)
{
    SP += 1;
    // printf("Pushing SP %x: %x\n", SP, v);
    p->D[SP] = v;
}

static inline uint8_t pop_sp_u8(mcs51_t* p)
{
    // printf("Popping SP %x: %x\n", SP, p->D[SP]);
    return p->D[SP--]; // Post-decrement
}

static inline void push_sp_u16(mcs51_t* p, uint16_t v)
{
    push_sp_u8(p, v >> 0); // LOW
    push_sp_u8(p, v >> 8); // HIGH
}

static inline uint16_t pop_sp_u16(mcs51_t* p)
{
    uint16_t high = pop_sp_u8(p);
    uint16_t low = pop_sp_u8(p);
    return (high << 8) | (low << 0);
}

static inline uint8_t bit_mask(uint8_t bit)
{
    return 0x01 << (bit % 8);
}

static inline uint8_t bit_byte_index(uint8_t bit)
{
    const uint8_t bit_addressable_memory_start = 0x20;
    uint8_t offset = 0;

    // Regular RAM
    if (bit < SFR_BASE_ADDRESS)
        offset = bit_addressable_memory_start + (bit / 8);
    // SFRs
    else
        offset = bit - (bit % 8);

    return offset;
}

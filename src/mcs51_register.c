/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#include "mcs51_register.h"
#include "mcs51.h"
#include "opcode_map_gen.h"
#include "sfr_definitions_gen.h"
#include "sfr_map_gen.h"
#include <assert.h>
#include <stdio.h>

void mcs51_register_opcodes(mcs51_t* p)
{
    assert(sizeof(p->opcode_map) == sizeof(opcode_map));

    for (unsigned int i = 0; i < OPCODE_MAP_SIZE; i++)
    {
        p->opcode_map[i] = opcode_map[i];
    }
}

static void noop(sfr_t* sfr, mcs51_t* p)
{
    //    if (sfr->name != 0)
    //        printf("SFR write access to %s: %x\n", sfr->name, p->D[sfr->address]);
    //    else
    //        printf("SFR write access to 0x%x: %x\n", sfr->address, p->D[sfr->address]);
}

static void on_write_sbuf(sfr_t* sfr, mcs51_t* p)
{
    p->_sfr_dirty_sbuf = true;
}

static void on_read_write_ie(sfr_t* sfr, mcs51_t* p)
{
    p->_instruction_register.accessed_sfr_ie = true;
}

static void on_read_write_ip(sfr_t* sfr, mcs51_t* p)
{
    p->_instruction_register.accessed_sfr_ip = true;
}

void mcs51_register_sfrs(mcs51_t* p)
{
    assert(sizeof(p->sfr_map) == sizeof(sfr_map));

    for (unsigned int i = 0; i < SFR_MAP_SIZE; i++)
    {
        p->sfr_map[i] = sfr_map[i];
        p->sfr_map[i].address = i;
        p->sfr_map[i].on_read = &noop;
        p->sfr_map[i].on_write = &noop;
    }

    p->sfr_map[SFR_SBUF].on_write = &on_write_sbuf;

    p->sfr_map[SFR_IE].on_write = &on_read_write_ie;
    p->sfr_map[SFR_IE].on_read = &on_read_write_ie;

    p->sfr_map[SFR_IP].on_write = &on_read_write_ip;
    p->sfr_map[SFR_IP].on_read = &on_read_write_ip;
}

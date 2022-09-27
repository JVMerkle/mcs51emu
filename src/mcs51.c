/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#include "mcs51.h"
#include "mcs51_helpers.h"
#include "mcs51_internal.h"
#include "mcs51_register.h"
#include "opcode.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void mcs51_timer_cycle(mcs51_t* p);
static void mcs51_set_address_latch_enable(mcs51_t* p);
static void mcs51_reset_address_latch_enable(mcs51_t* p);

static void msc51_idle(mcs51_t* p);

static void msc51_s1p1(mcs51_t* p);
static void msc51_s1p2(mcs51_t* p);
static void msc51_s2p1(mcs51_t* p);
static void msc51_s2p2(mcs51_t* p);
static void msc51_s3p1(mcs51_t* p);
static void msc51_s3p2(mcs51_t* p);
static void msc51_s4p1(mcs51_t* p);
static void msc51_s4p2(mcs51_t* p);
static void msc51_s5p1(mcs51_t* p);
static void msc51_s5p2(mcs51_t* p);
static void msc51_s6p1(mcs51_t* p);
static void msc51_s6p2(mcs51_t* p);

static void on_serial_tx_default_handler(char c)
{
    putc(c, stdout);
    fflush(stdout);
}

void mcs51_init(mcs51_t* p)
{
    mcs51_register_opcodes(p);
    mcs51_register_sfrs(p);

    nvic_init(&p->_nvic);

    p->_state_phases[0] = &msc51_s1p1;
    p->_state_phases[1] = &msc51_s1p2;
    p->_state_phases[2] = &msc51_s2p1;
    p->_state_phases[3] = &msc51_s2p2;
    p->_state_phases[4] = &msc51_s3p1;
    p->_state_phases[5] = &msc51_s3p2;
    p->_state_phases[6] = &msc51_s4p1;
    p->_state_phases[7] = &msc51_s4p2;
    p->_state_phases[8] = &msc51_s5p1;
    p->_state_phases[9] = &msc51_s5p2;
    p->_state_phases[10] = &msc51_s6p1;
    p->_state_phases[11] = &msc51_s6p2;

    p->_osc_frequency_hertz = 11059200;

    mcs51_reset(p);

    p->_on_serial_tx = &on_serial_tx_default_handler;
    p->_abort_on_unimplemented_opcode = true;
}

void mcs51_reset(mcs51_t* p)
{
    nvic_reset(&p->_nvic);

    p->D[SFR_SP] = 0x07;
    p->D[SFR_TCON] = 0x00;

    p->D[SFR_PCON] &= 0b00100000; // Bit 6 is don't care
    p->D[SFR_PCON] |= 0b00010000; // Set bit 5

    p->D[SFR_TMOD] = 0x00;
    p->D[SFR_TH0] = 0x00;
    p->D[SFR_TL0] = 0x00;
    p->D[SFR_TH1] = 0x00;
    p->D[SFR_TL1] = 0x00;
    p->D[SFR_SCON] = 0x00;
    p->D[SFR_AUXR] &= ~0b11;

    p->D[SFR_BRL] = 0x00;
    p->D[SFR_BDRCON] &= 0b11100000;
    p->D[SFR_SADDR] = 0x00;
    p->D[SFR_SADEN] = 0x00;
}

void mcs51_print_state(mcs51_t* p)
{
    const uint8_t states = 6;
    uint8_t state = (p->_osc_periods / 2) % states + 1;
    uint8_t phase = p->_osc_periods % 2 + 1;
    printf("S%dP%d", state, phase);
}

void mcs51_print_current_instruction(mcs51_t* p)
{
    opcode_t opcode = p->_instruction_register.opcode;

    printf("0x%04x: ", p->PC);
    opcode_print(&opcode);

    if (opcode.bytes > 1)
    {
        printf(" (%02x", p->_instruction_register.args[0]);
        if (opcode.bytes > 2)
            printf(", %02x", p->_instruction_register.args[1]);
        if (opcode.bytes > 3)
            printf(", %02x", p->_instruction_register.args[2]);
        printf(")");
    }

    printf("\n");
}

double msc51_execution_time_ms(mcs51_t* p)
{
    return (double) p->_osc_periods * 1000. / p->_osc_frequency_hertz;
}

void msc51_do_machine_cycle(mcs51_t* p)
{
    for (uint8_t i = 0; i < 12; i++)
        msc51_do_osc_period(p);
}

/**
 * Typically, arithmetic and logical operations take place during
 * Phase 1 and internal register-to-register transfers take place during Phase 2.
 */
void msc51_do_osc_period(mcs51_t* p)
{
    p->_state_phases[p->_osc_periods % 12](p);
    p->_osc_periods++;
}

//////////// PHASES BEGIN ////////////

void msc51_s1p1(mcs51_t* p)
{
}

void msc51_s1p2(mcs51_t* p)
{
    mcs51_set_address_latch_enable(p);

    /// Select a pending interrupt if applicable
    nvic_run_interrupt_controller(&p->_nvic, p);

    // Latch opcode into instruction register (Fetch)
    if (p->_instruction_register.opcode.cycles == 0)
    {
        uint8_t opcode = p->C[p->PC];
        mcs51_reset_and_load_instruction_register(p, p->opcode_map[opcode]);

        // Note: The instruction register arguments are currently unused
        mcs51_load_instruction_register_arguments(p, p->C[p->PC + 1], p->C[p->PC + 2], p->C[p->PC + 3]);

        p->PC++; // The opcode actor will pop the arguments from the PC
    }
}

void msc51_s2p1(mcs51_t* p)
{
}

void msc51_s2p2(mcs51_t* p)
{
    mcs51_reset_address_latch_enable(p);
}

void msc51_s3p1(mcs51_t* p)
{
}

void msc51_s3p2(mcs51_t* p)
{
}

void msc51_s4p1(mcs51_t* p)
{
}

void msc51_s4p2(mcs51_t* p)
{
    mcs51_set_address_latch_enable(p);

    assert(p->_instruction_register.opcode.actor);
    p->_instruction_register.opcode.actor(p);

    // Workaround: Execute in first instruction cycle only
    p->_instruction_register.opcode.actor = &msc51_idle;

    p->_instruction_register.opcode.cycles--;
}

void msc51_s5p1(mcs51_t* p)
{
}

void msc51_s5p2(mcs51_t* p)
{
    mcs51_reset_address_latch_enable(p);
    nvic_latch_interrupt_flags(&p->_nvic, p);
}

void msc51_s6p1(mcs51_t* p)
{
}

void msc51_s6p2(mcs51_t* p)
{
    mcs51_timer_cycle(p);
}

//////////// PHASES END ////////////

void mcs51_reset_and_load_instruction_register(mcs51_t* p, opcode_t opcode)
{
    p->_instruction_register = (instruction_register_t){.opcode = opcode};
}

void mcs51_load_instruction_register_arguments(mcs51_t* p, uint8_t arg1, uint8_t arg2, uint8_t arg3)
{
    p->_instruction_register.args[0] = arg1;
    p->_instruction_register.args[1] = arg2;
    p->_instruction_register.args[2] = arg3;
}

void mcs51_set_address_latch_enable(mcs51_t* p)
{
    p->_ale = !(p->D[SFR_AUXR] & SFR_AUXR_A0_Msk);
}

void mcs51_reset_address_latch_enable(mcs51_t* p)
{
    p->_ale = false;
}

void msc51_idle(mcs51_t* p)
{
    // NOP
}

void mcs51_timer_cycle(mcs51_t* p)
{
    // Timer 0 running
    // Mode 0 is a 13 bit Timer mode and uses 8 bits of high byte and 5 bit prescaler of low byte.
    // The value that the Timer can update in mode0 is from 0000H to 1FFFH. The 5 bits of lower byte
    // append with the bits of higher byte. The Timer rolls over from 1FFFH to 0000H to raise the Timer flag.
    if (p->D[SFR_TCON] & SFR_TCON_TR0_Msk)
    {
        uint8_t mode = ((p->D[SFR_TMOD] & SFR_TMOD_T0M1_Msk) >> SFR_TMOD_T0M1_Pos) << 1 | ((p->D[SFR_TMOD] & SFR_TMOD_T0M0_Msk) >> SFR_TMOD_T0M0_Pos);

        // Timer 0 Mode 0 (13-bit)
        if (mode == 0)
        {
            // Over-run TL0 ("prescaler" overrun)
            if (++p->D[SFR_TL0] > 0b11111) // Pre-increment
            {
                p->D[SFR_TL0] = 0x00;

                // Over-run TH0
                if (++p->D[SFR_TH0] == 0x00) // Pre-increment
                {
                    if (p->D[SFR_IE] & SFR_IE_EA_Msk && p->D[SFR_IE] & SFR_IE_ET0_Msk)
                    {
                        p->D[SFR_TCON] |= SFR_TCON_TF0_Msk;
                    }
                }
            }
        }
        // Timer 0 Mode 1 (16-bit)
        else if (mode == 1)
        {
            // Over-run TL0
            if (++p->D[SFR_TL0] == 0x00) // Pre-increment
            {
                // Over-run TH0
                if (++p->D[SFR_TH0] == 0x00) // Pre-increment
                {
                    if (p->D[SFR_IE] & SFR_IE_EA_Msk && p->D[SFR_IE] & SFR_IE_ET0_Msk)
                    {
                        p->D[SFR_TCON] |= SFR_TCON_TF0_Msk;
                    }
                }
            }
        } else
        {
            fprintf(stderr, "Unimplemented timer 0 mode: %d\n", mode);
            abort();
        }
    }


    // Timer 1 running
    if (p->D[SFR_TCON] & SFR_TCON_TR1_Msk)
    {
        uint8_t mode = ((p->D[SFR_TMOD] & SFR_TMOD_T1M1_Msk) >> SFR_TMOD_T1M1_Pos) << 1 | ((p->D[SFR_TMOD] & SFR_TMOD_T1M0_Msk) >> SFR_TMOD_T1M0_Pos);

        // Timer 1 Mode 2 (auto-reload)
        if (mode == 2)
        {
            // Over-run
            if (++p->D[SFR_TL1] == 0x00) // Pre-increment
            {
                // Reload
                p->D[SFR_TL1] = p->D[SFR_TH1];

                if (p->D[SFR_IE] & SFR_IE_EA_Msk && p->D[SFR_IE] & SFR_IE_ET1_Msk)
                {
                    p->D[SFR_TCON] |= SFR_TCON_TF1_Msk;
                }

                uint8_t serial_mode = ((p->D[SFR_SCON] & SFR_SCON_SM0_Msk) >> SFR_SCON_SM0_Pos) << 1 | ((p->D[SFR_SCON] & SFR_SCON_SM1_Msk) >> SFR_SCON_SM1_Pos);

                // Serial mode 1: 8-bit, 1 stop
                if (serial_mode == 1)
                {
                    // SBUF SFR is dirty
                    if (p->_sfr_dirty_sbuf)
                    {
                        p->_sfr_dirty_sbuf = false;
                        p->D[SFR_SCON] |= SFR_SCON_TI_Msk; // Set the Transmit Interrupt flag (cleared by software)

                        p->_on_serial_tx((char) p->D[SFR_SBUF]);
                    }
                } else
                {
                    fprintf(stderr, "Unimplemented serial mode: %d\n", serial_mode);
                    abort();
                }
            }
        } else
        {
            fprintf(stderr, "Unimplemented timer 1 mode: %d\n", mode);
            abort();
        }
    }
}

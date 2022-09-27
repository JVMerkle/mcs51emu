/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#include "nvic.h"
#include "mcs51.h"
#include "mcs51_helpers.h"
#include "mcs51_internal.h"
#include "sfr_definitions_gen.h"

static void nvic_jump_to_isr(nvic_t* nvic, mcs51_t* p, interrupt_t interrupt);

void nvic_init(nvic_t* nvic)
{
    /// MSB [ RI/TI | TF1 | IE1 | TF0 | IE0 ] LSB
    nvic->map[0] = (interrupt_t){.name = "INT0 (IE0)", .bit_mask = SFR_IE_EX0_Msk, .vector = 0x0003, .sfr_address = SFR_TCON, .sfr_bit_mask = SFR_TCON_IE0_Msk, .clears_flag = true};
    nvic->map[1] = (interrupt_t){.name = "Timer 0 (TF0)", .bit_mask = SFR_IE_ET0_Msk, .vector = 0x000B, .sfr_address = SFR_TCON, .sfr_bit_mask = SFR_TCON_TF0_Msk, .clears_flag = true};
    nvic->map[2] = (interrupt_t){.name = "INT1 (IE1)", .bit_mask = SFR_IE_EX1_Msk, .vector = 0x0013, .sfr_address = SFR_TCON, .sfr_bit_mask = SFR_TCON_IE1_Msk, .clears_flag = true};
    nvic->map[3] = (interrupt_t){.name = "Timer 1 (TF1)", .bit_mask = SFR_IE_ET1_Msk, .vector = 0x001B, .sfr_address = SFR_TCON, .sfr_bit_mask = SFR_TCON_TF1_Msk, .clears_flag = true};
    nvic->map[4] = (interrupt_t){.name = "Serial (TI/RI)", .bit_mask = SFR_IE_ES_Msk, .vector = 0x0023, .sfr_address = SFR_SCON, .sfr_bit_mask = SFR_SCON_RI_Msk | SFR_SCON_TI_Msk};
}

void nvic_reset(nvic_t* nvic)
{
    nvic->_isr_pending = 0;
    nvic->_isr_active_msk = 0;
}

/**
 * In operation all the interrupt flags are latched into the interrupt control system during State 5
 * of every machine cycle. The samples are polled during the following machine cycle-If the flag for an
 * enabled interrupt is found to be set (l), the interrupt system generates an LCALL to the appropriate
 * location in Program Memory, unless some other condition blocks the interrupt. Several conditions can
 * block an interrupt, among them that an interrupt of equal or higher priority level is already in progress.
 */
void nvic_latch_interrupt_flags(nvic_t* nvic, mcs51_t* p)
{
    nvic->_isr_pending = 0;

    /// MSB [ RI/TI | TF1 | IE1 | TF0 | IE0 ] LSB
    nvic->_isr_pending |= (p->D[SFR_TCON] & SFR_TCON_IE0_Msk) >> SFR_TCON_IE0_Pos << 0;
    nvic->_isr_pending |= (p->D[SFR_TCON] & SFR_TCON_TF0_Msk) >> SFR_TCON_TF0_Pos << 1;
    nvic->_isr_pending |= (p->D[SFR_TCON] & SFR_TCON_IE1_Msk) >> SFR_TCON_IE1_Pos << 2;
    nvic->_isr_pending |= (p->D[SFR_TCON] & SFR_TCON_TF1_Msk) >> SFR_TCON_TF1_Pos << 3;
    nvic->_isr_pending |= (p->D[SFR_SCON] & SFR_SCON_RI_Msk) >> SFR_SCON_RI_Pos << 4;
    nvic->_isr_pending |= (p->D[SFR_SCON] & SFR_SCON_TI_Msk) >> SFR_SCON_TI_Pos << 4;
}


// TODO SFR_IP Interrupt Prios

// Convert an (interrupt) mask to the bit number. Does not work for an empty mask.
static uint8_t nvic_interrupt_mask_to_bit_number(uint8_t interrupt_mask)
{
    uint8_t interrupt_number = 0;
    while ((interrupt_mask >> (interrupt_number + 1)) > 0)
        interrupt_number++;

    return interrupt_number;
}

/// Scan from LSB (High-Prio) to MSB (Low-Prio) bits
static uint8_t nvic_scan(uint8_t interrupt_bit_mask)
{
    uint8_t mask = 1;

    while (mask < SFR_IE_EA_Msk && (interrupt_bit_mask & mask) == 0)
        mask <<= 1;

    return interrupt_bit_mask & mask;
}

static uint8_t nvic_priority_scan(uint8_t priority_mask, uint8_t interrupt_bit_mask)
{
    uint8_t prio_mask_high = nvic_scan(priority_mask & interrupt_bit_mask);
    if (prio_mask_high)
    {
        return prio_mask_high;
    }

    // Lower priority
    return nvic_scan(interrupt_bit_mask);
}

static void nvic_select_next_interrupt(nvic_t* nvic, mcs51_t* p, uint8_t interrupt_bit_mask)
{
    // Scan for the highest priority interrupt
    uint8_t interrupt_mask = nvic_priority_scan(p->D[SFR_IP], interrupt_bit_mask);

    if (interrupt_mask)
    {
        // The selected interrupt is active already (running or not, does not matter)
        if (nvic->_isr_active_msk & interrupt_mask)
            return;

        uint8_t interrupt_number = nvic_interrupt_mask_to_bit_number(interrupt_mask);
        interrupt_t interrupt = nvic->map[interrupt_number];

        // Clear flags if applicable
        if (interrupt.clears_flag)
        {
            const uint8_t sfr_address = interrupt.sfr_address;
            const uint8_t sfr_bit_mask = interrupt.sfr_bit_mask;
            p->D[sfr_address] &= ~sfr_bit_mask;
        }

        nvic_jump_to_isr(nvic, p, interrupt);
    }
}

void nvic_run_interrupt_controller(nvic_t* nvic, mcs51_t* p)
{
    uint8_t interrupt_enable = p->D[SFR_IE];
    uint8_t pending_and_enabled = nvic->_isr_pending & interrupt_enable;

    // Current instruction complete AND all interrupts are enabled AND current instruction is not RETI
    // AND current instruction does not have access to IP or IE
    if (p->_instruction_register.opcode.cycles == 0
        && p->_instruction_register.opcode.code != 0x32
        && (interrupt_enable & SFR_IE_EA_Msk)
        && !p->_instruction_register.accessed_sfr_ie
        && !p->_instruction_register.accessed_sfr_ip)
    {
        nvic_select_next_interrupt(nvic, p, nvic->_isr_active_msk | pending_and_enabled);
    }
}

void nvic_reti(nvic_t* nvic, mcs51_t* p)
{
    /// Clear the currently running ISR
    nvic->_isr_active_msk &= ~(nvic->_isr_running_msk);

    // Scan for the highest priority interrupt
    nvic->_isr_running_msk = nvic_priority_scan(p->D[SFR_IP], nvic->_isr_active_msk);
}

static void nvic_inserted_LJMP(mcs51_t* p)
{
    push_sp_u16(p, p->PC);
    p->PC = p->_nvic._ljmp_vector;
}

void nvic_jump_to_isr(nvic_t* nvic, mcs51_t* p, interrupt_t interrupt)
{
    nvic->_isr_running_msk = interrupt.bit_mask;
    nvic->_isr_active_msk |= nvic->_isr_running_msk;
    nvic->_ljmp_vector = interrupt.vector;

    mcs51_reset_and_load_instruction_register(p, p->opcode_map[0x02]);                                                  // LJMP addr16
    mcs51_load_instruction_register_arguments(p, (interrupt.vector >> 0) & 0xFF, (interrupt.vector >> 8) & 0xFF, 0x00); // LJMP addr16

    p->_instruction_register.opcode.mnemonic = "NVIC LJMP";      // Override the mnemonic
    p->_instruction_register.opcode.actor = &nvic_inserted_LJMP; // Override the actor
}

/**
 * SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
 * SPDX-License-Identifier: MIT
 */

#include "mcs51.h"
#include "mcs51_helpers.h"

#define IMPL(name) void name(mcs51_t* p)

IMPL(NOP)
{
}

IMPL(INC_direct)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] += 1;

    check_sfr_write_access(p, direct);
}

IMPL(INC_DPTR)
{
    if (++p->D[SFR_DPL] == 0) // Pre-increment
    {
        p->D[SFR_DPH] += 1;
    }
}

IMPL(INC_A)
{
    ACC += 1;
}

IMPL(INC_R0)
{
    R0 += 1;
}

IMPL(INC_R1)
{
    R1 += 1;
}

IMPL(INC_R2)
{
    R2 += 1;
}

IMPL(INC_R3)
{
    R3 += 1;
}

IMPL(INC_R4)
{
    R4 += 1;
}

IMPL(INC_R5)
{
    R5 += 1;
}

IMPL(INC_R6)
{
    R6 += 1;
}

IMPL(INC_R7)
{
    R7 += 1;
}

IMPL(DEC_R0)
{
    R0 -= 1;
}

IMPL(DEC_R1)
{
    R1 -= 1;
}

IMPL(DEC_R2)
{
    R2 -= 1;
}

IMPL(DEC_direct)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] -= 1;

    check_sfr_write_access(p, direct);
}

IMPL(ADD_A_immed)
{
    uint8_t immed = pop_pc_u8(p);
    ACC += immed;
}

IMPL(ADD_A_direct)
{
    uint8_t direct = pop_pc_u8(p);
    ACC += p->D[direct];

    check_sfr_read_access(p, direct);
}

IMPL(ADD_A_R0)
{
    ACC += R0;
}

IMPL(ADD_A_R1)
{
    ACC += R1;
}

IMPL(ADD_A_R2)
{
    ACC += R2;
}

IMPL(ADD_A_R3)
{
    ACC += R3;
}

IMPL(ADD_A_R4)
{
    ACC += R4;
}

IMPL(ADD_A_R5)
{
    ACC += R5;
}

IMPL(ADD_A_R6)
{
    ACC += R6;
}

IMPL(ADD_A_R7)
{
    ACC += R7;
}

IMPL(SUBB_A_R6)
{
    uint8_t underflow = ACC < (GET_C() + R6);
    ACC = ACC - GET_C() - R6;
    if (underflow)
        SET_C();
    else
        CLEAR_C();
}

IMPL(SUBB_A_R7)
{
    uint8_t underflow = ACC < (GET_C() + R7);
    ACC = ACC - GET_C() - R7;
    if (underflow)
        SET_C();
    else
        CLEAR_C();
}

/**
 * The MUL instruction multiplies the unsigned 8-bit integer in the accumulator and the unsigned 8-bit
 * integer in the B register producing a 16-bit product. The low-order byte of the product is returned
 * in the accumulator. The high-order byte of the product is returned in the B register. The OV flag is set
 * if the product is greater than 255 (0FFh), otherwise it is cleared. The carry flag is always cleared.
 */
IMPL(MUL_AB)
{
    uint16_t product = (uint16_t) p->D[SFR_ACC] * p->D[SFR_B];

    p->D[SFR_ACC] = (product >> 0) & 0xFF;
    p->D[SFR_B] = (product >> 8) & 0xFF;

    if (product > 0xFF)
        p->D[SFR_PSW] |= SFR_PSW_OV_Msk;
    else
        p->D[SFR_PSW] &= ~SFR_PSW_OV_Msk;

    p->D[SFR_PSW] &= ~SFR_PSW_C_Msk;
}

IMPL(SWAP_A)
{
    ACC = ((ACC >> 4) & 0x0F) | ((ACC << 4) & 0xF0);
}

IMPL(RL_A)
{
    if (ACC & 0b10000000)
        ACC = (ACC << 1) | 0b1;
    else
        ACC = (ACC << 1);
}

IMPL(CPL_bit)
{
    uint8_t bit = pop_pc_u8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    if ((p->D[byte_idx] & mask) == 0)
        p->D[byte_idx] |= mask;
    else
        p->D[byte_idx] &= ~mask;

    check_sfr_write_access(p, byte_idx);
}

IMPL(SETB_bit)
{
    uint8_t bit = pop_pc_u8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    p->D[byte_idx] |= mask;

    check_sfr_write_access(p, byte_idx);
}

IMPL(CLR_C)
{
    CLEAR_C();
}

IMPL(CLR_A)
{
    ACC = 0;
}

IMPL(CLR_bit)
{
    uint8_t bit = pop_pc_u8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    p->D[byte_idx] &= ~mask;

    check_sfr_write_access(p, byte_idx);
}

IMPL(MOV_C_bit)
{
    uint8_t bit = pop_pc_u8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    if (p->D[byte_idx] & mask)
        SET_C();
    else
        CLEAR_C();
}

IMPL(MOV_R1_direct)
{
    uint8_t direct = pop_pc_u8(p);
    R1 = p->D[direct];

    check_sfr_read_access(p, direct);
}

IMPL(MOV_direct_direct)
{
    uint8_t direct1 = pop_pc_u8(p);
    uint8_t direct2 = pop_pc_u8(p);
    p->D[direct1] = p->D[direct2];

    check_sfr_read_access(p, direct2);
    check_sfr_write_access(p, direct1);
}

IMPL(MOV_direct_A)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = ACC;

    check_sfr_write_access(p, direct);
}

IMPL(MOV_direct_R0)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = R0;

    check_sfr_write_access(p, direct);
}

IMPL(MOV_direct_R1)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = R1;

    check_sfr_write_access(p, direct);
}

IMPL(MOV_direct_R2)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = R2;

    check_sfr_write_access(p, direct);
}

IMPL(MOV_direct_R3)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = R3;

    check_sfr_write_access(p, direct);
}

IMPL(MOV_direct_AtR0)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = p->D[R0];

    check_sfr_write_access(p, direct);
}

IMPL(MOV_direct_AtR1)
{
    uint8_t direct = pop_pc_u8(p);
    p->D[direct] = p->D[R1];

    check_sfr_write_access(p, direct);
}

IMPL(MOV_R2_A)
{
    R2 = ACC;
}

IMPL(MOV_R3_A)
{
    R3 = ACC;
}

IMPL(MOV_R4_A)
{
    R4 = ACC;
}

IMPL(MOV_R6_A)
{
    R6 = ACC;
}

IMPL(MOV_R1_immed)
{
    uint8_t immed = pop_pc_u8(p);
    R1 = immed;
}

IMPL(MOV_R3_immed)
{
    uint8_t immed = pop_pc_u8(p);
    R3 = immed;
}

IMPL(MOV_R4_immed)
{
    uint8_t immed = pop_pc_u8(p);
    R4 = immed;
}

IMPL(MOV_R7_immed)
{
    uint8_t immed = pop_pc_u8(p);
    R7 = immed;
}

IMPL(MOV_direct_immed)
{
    uint8_t direct = pop_pc_u8(p);
    uint8_t immed = pop_pc_u8(p);

    p->D[direct] = immed;

    check_sfr_write_access(p, direct);
}

IMPL(MOV_A_direct)
{
    uint8_t direct = pop_pc_u8(p);
    ACC = p->D[direct];

    check_sfr_read_access(p, direct);
}

IMPL(MOV_A_AtR0)
{
    uint16_t indirect = to_indirect_address(R1);
    ACC = p->D[indirect];
}

IMPL(MOV_A_AtR1)
{
    uint16_t indirect = to_indirect_address(R0);
    ACC = p->D[indirect];
}

IMPL(MOV_A_immed)
{
    uint8_t immed = pop_pc_u8(p);

    ACC = immed;
}

IMPL(MOV_R0_A)
{
    R0 = ACC;
}

IMPL(MOV_R1_A)
{
    R1 = ACC;
}

IMPL(MOV_R0_direct)
{
    uint8_t direct = pop_pc_u8(p);

    R0 = p->D[direct];

    check_sfr_read_access(p, direct);
}

IMPL(MOV_R0_immed)
{
    uint8_t immed = pop_pc_u8(p);

    R0 = immed;
}

IMPL(MOV_AtR0_A)
{
    uint16_t indirect = to_indirect_address(R0);
    p->D[indirect] = ACC;
}

IMPL(MOV_AtR1_A)
{
    uint16_t indirect = to_indirect_address(R1);
    p->D[indirect] = ACC;
}

IMPL(MOV_AtR0_immed)
{
    uint8_t immed = pop_pc_u8(p);
    uint16_t indirect = to_indirect_address(R0);
    p->D[indirect] = immed;
}

IMPL(MOV_A_R0)
{
    ACC = R0;
}

IMPL(MOV_A_R1)
{
    ACC = R1;
}

IMPL(MOV_A_R2)
{
    ACC = R2;
}

IMPL(MOV_A_R3)
{
    ACC = R3;
}

IMPL(MOV_A_R4)
{
    ACC = R4;
}

IMPL(MOV_A_R5)
{
    ACC = R5;
}

IMPL(MOV_A_R6)
{
    ACC = R6;
}

IMPL(MOV_A_R7)
{
    ACC = R7;
}

IMPL(MOV_R5_A)
{
    R5 = ACC;
}

IMPL(MOV_R7_A)
{
    R7 = ACC;
}

IMPL(MOV_DPTR_immed)
{
    uint16_t immed = pop_pc_u16(p);

    p->D[SFR_DPL] = immed;
    p->D[SFR_DPH] = immed >> 8;
}

IMPL(MOVX_A_AtDPTR)
{

    uint16_t dptr = (((uint16_t) p->D[SFR_DPH]) << 8) | p->D[SFR_DPL];
    ACC = p->X[dptr];
}

IMPL(MOVX_AtDPTR_A)
{

    uint16_t dptr = (((uint16_t) p->D[SFR_DPH]) << 8) | p->D[SFR_DPL];
    p->X[dptr] = ACC;
}

IMPL(MOVC_A_AtAPlusDPTR)
{

    uint16_t dptr = (((uint16_t) p->D[SFR_DPH]) << 8) | p->D[SFR_DPL];
    ACC = p->C[ACC + dptr];
}

IMPL(ANL_A_AtR1)
{
    uint16_t indirect = to_indirect_address(R1);
    ACC &= p->D[indirect];
}

IMPL(ANL_C_Negbit)
{
    uint8_t bit = pop_pc_u8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    if (GET_C() && !(p->D[byte_idx] & mask))
        SET_C();
    else
        CLEAR_C();
}

IMPL(ANL_A_direct)
{
    uint8_t direct = pop_pc_u8(p);

    ACC &= p->D[direct];

    check_sfr_read_access(p, direct);
}

IMPL(ANL_A_immed)
{
    uint8_t immed = pop_pc_u8(p);

    ACC &= immed;
}

IMPL(ANL_direct_A)
{
    uint8_t direct = pop_pc_u8(p);

    p->D[direct] &= ACC;

    check_sfr_write_access(p, direct);
}

IMPL(ANL_A_R2)
{
    ACC &= R2;
}

IMPL(ANL_A_R6)
{
    ACC &= R6;
}

IMPL(ORL_A_R1)
{
    ACC |= R1;
}

IMPL(ORL_A_R6)
{
    ACC |= R6;
}

IMPL(ORL_direct_immed)
{
    uint8_t direct = pop_pc_u8(p);
    uint8_t immed = pop_pc_u8(p);

    p->D[direct] |= immed;

    check_sfr_write_access(p, direct);
}

IMPL(CJNE_AtR1_immed_offset)
{
    uint8_t immed = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    uint16_t indirect = to_indirect_address(R0);
    uint8_t at = p->D[indirect];
    if (at != immed)
        p->PC = p->PC + offset;

    if (at < immed)
        SET_C();
    else
        CLEAR_C();
}

IMPL(CJNE_R0_immed_offset)
{
    uint8_t immed = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    if (R0 != immed)
        p->PC = p->PC + offset;

    if (R0 < immed)
        SET_C();
    else
        CLEAR_C();
}

IMPL(CJNE_R2_immed_offset)
{
    uint8_t immed = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    if (R2 != immed)
        p->PC = p->PC + offset;

    if (R2 < immed)
        SET_C();
    else
        CLEAR_C();
}

IMPL(CJNE_R4_immed_offset)
{
    uint8_t immed = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    if (R4 != immed)
        p->PC = p->PC + offset;

    if (R4 < immed)
        SET_C();
    else
        CLEAR_C();
}

IMPL(CJNE_A_immed_offset)
{
    uint8_t immed = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    if (ACC != immed)
        p->PC = p->PC + offset;

    if (ACC < immed)
        SET_C();
    else
        CLEAR_C();
}

IMPL(CJNE_A_direct_offset)
{
    uint8_t direct = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    if (ACC != p->D[direct])
        p->PC = p->PC + offset;

    if (ACC < p->D[direct])
        SET_C();
    else
        CLEAR_C();

    check_sfr_read_access(p, direct);
}

IMPL(JB_bit_offset)
{
    uint8_t bit = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    if (p->D[byte_idx] & mask)
        p->PC = p->PC + offset;
}

IMPL(JC_offset)
{
    int8_t offset = pop_pc_s8(p);

    if (GET_C() == 1)
        p->PC = p->PC + offset;
}

IMPL(JZ_offset)
{
    int8_t offset = pop_pc_s8(p);

    if (ACC == 0)
        p->PC += offset;
}

IMPL(JNZ_offset)
{
    int8_t offset = pop_pc_s8(p);

    if (ACC != 0)
        p->PC += offset;
}

IMPL(DJNZ_R7_offset)
{
    int8_t offset = pop_pc_s8(p);
    // p->PC += 2; // This should be 3? See Keil documentation...
    R7 -= 1;
    if (R7 != 0)
        p->PC += offset;
}

IMPL(JNB_bit_offset)
{
    uint8_t bit = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    if ((p->D[byte_idx] & mask) == 0)
        p->PC += offset;
}

/**
 * TODO: When this instruction is used to modify an output port, the value used as the port data is read
 * from the output data latch, not the input pins of the port.
 */
IMPL(JBC_bit_offset)
{
    uint8_t bit = pop_pc_u8(p);
    int8_t offset = pop_pc_s8(p);

    uint8_t mask = bit_mask(bit);
    uint8_t byte_idx = bit_byte_index(bit);

    if (p->D[byte_idx] & mask)
    {
        p->D[byte_idx] &= ~mask; // Clear bit
        check_sfr_write_access(p, byte_idx);
        p->PC += offset;
    }
}

IMPL(SJMP_offset)
{
    int8_t offset = pop_pc_s8(p);

    p->PC += offset;
}

IMPL(LJMP_addr16)
{
    uint16_t addr16 = pop_pc_u16(p);
    p->PC = addr16;
}

IMPL(AJMP_addr11)
{
    // A10-A9-A8-1-0-0-0-1		A7-A6-A5-A4-A3-A2-A1-A0
    uint8_t code = p->_instruction_register.opcode.code;
    uint8_t addr11 = pop_pc_u8(p);

    uint16_t a = (uint16_t) code << 3;
    a |= addr11;

    p->PC = p->PC & ~0x3FF; // Clear bit 10-0
    p->PC |= a;
}

IMPL(ACALL_addr11)
{
    // A10-A9-A8-1-0-0-0-1		A7-A6-A5-A4-A3-A2-A1-A0
    uint8_t code = p->_instruction_register.opcode.code;
    uint16_t addr11 = pop_pc_u8(p);
    addr11 |= ((uint16_t) code >> 5);

    push_sp_u16(p, p->PC);

    p->PC = p->PC & ~0x3FF; // Clear bit 10-0
    p->PC |= addr11;
}

IMPL(LCALL_addr16)
{
    uint16_t addr16 = pop_pc_u16(p);

    push_sp_u16(p, p->PC);

    p->PC = addr16;
}

IMPL(RET)
{
    p->PC = pop_sp_u16(p);
}

IMPL(RETI)
{
    p->PC = pop_sp_u16(p);
    nvic_reti(&p->_nvic, p);
}

IMPL(PUSH_direct)
{
    uint8_t direct = pop_pc_u8(p);

    push_sp_u8(p, p->D[direct]);

    check_sfr_read_access(p, direct); // Todo: Is this even possible?
}

IMPL(POP_direct)
{
    uint8_t direct = pop_pc_u8(p);

    p->D[direct] = pop_sp_u8(p);

    check_sfr_write_access(p, direct); // Todo: Is this even possible?
}

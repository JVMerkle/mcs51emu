#include <mcs51.h>
#include <stdio.h>

#include "sfr_definitions_gen.h"
#include <assert.h>
#include <string.h>

/// Test helper macro
typedef struct test_cfg_t {
    bool verbose;
} test_cfg_t;

static test_cfg_t s_test_cfg = {};

#define TEST(name) static bool name(test_cfg_t* test_cfg)

#define RUN_TEST(test_func)                            \
    do {                                               \
        printf("Running %-40s ", "" #test_func "..."); \
        bool ok = test_func(&s_test_cfg);              \
        code += (ok - 1);                              \
        printf("%s\n", ok ? "Ok" : "Fail");            \
    } while (false)

static void machine_cycle(test_cfg_t* test_cfg, mcs51_t* p)
{
    if (test_cfg->verbose)
        mcs51_print_current_instruction(p);
    msc51_do_machine_cycle(p);
}

static void run_until_nop(test_cfg_t* test_cfg, mcs51_t* p)
{
    do {
        machine_cycle(test_cfg, p);
    } while (p->_instruction_register.opcode.code != 0);
}

#define RUN_UNTIL_NOP() run_until_nop(&s_test_cfg, &proc)
#define MACHINE_CYCLE() machine_cycle(&s_test_cfg, &proc)

TEST(test_nop)
{
    mcs51_t proc = {.C = {0x00}}; // NOP
    mcs51_init(&proc);

    RUN_UNTIL_NOP();

    return proc._osc_periods == 12; // 1 machine cycle
}

/**
 * Exchange the content of FFh and FF00h.
 * MOV dptr, #0FF00h     ; take the address in dptr
 * MOVX a, @dptr         ; get the content of 0050h in a
 * MOV r0, 0FFh          ; save the content of 50h in r0
 * MOV 0FFh, a           ; move a to 50h
 * MOV a, r0             ; get content of 50h in a
 * MOVX @dptr, a         ; move it to 0050h
 */
TEST(test_data_xdata_exchange)
{
    mcs51_t proc = {.C = {0x90, 0xff, 0x00, 0xe0, 0xa8, 0xff, 0xf5, 0xff, 0xe8, 0xf0}};
    mcs51_init(&proc);

    proc.D[0xFF] = 0xAD;
    proc.X[0xFF00] = 0xDE;

    RUN_UNTIL_NOP();

    return proc.D[0xFF] == 0xDE && proc.X[0xFF00] == 0xAD;
}

/**
 * Store the higher nibble of r7 in to both nibbles of r6.
 * Mov a, r7          ; get the content in acc
 * Anl a, #0F0h       ; mask lower bit
 * Mov r6, a          ; send it to r6
 * Swap a             ; xchange upper and lower nibbles of acc
 * Orl a, r6          ; OR operation
 * Mov r6, a          ; finally load content in r6
 */
TEST(test_swap)
{
    mcs51_t proc = {.C = {0xef, 0x54, 0xf0, 0xfe, 0xc4, 0x4e, 0xfe}};
    mcs51_init(&proc);

    proc.D[0x07] = 0xBE; // Set R7
    proc.D[0x06] = 0x69; // Pollute R6

    RUN_UNTIL_NOP();

    return proc.D[0x06] == 0xBB;
}

TEST(test_swap2)
{
    mcs51_t proc = {.C = {0xc4}}; // SWAP
    mcs51_init(&proc);

    proc.D[SFR_ACC] = 0x5A;

    RUN_UNTIL_NOP();

    return proc.D[SFR_ACC] == 0xA5;
}

/**
 * Treat r6-r7 and r4-r5 as two 16 bit registers. Perform subtraction between them.
 * Store the result in 20h (lower byte) and 21h (higher byte).
 * Clr c              ; clear carry
 * Mov a, r4          ; get first lower byte
 * Subb a, r6         ; subtract it with other
 * Mov 20h, a         ; store the result
 * Mov a, r5          ; get the first higher byte
 * Subb a, r7         ; subtract from other
 * Mov 21h, a         ; store the higher byte
 */
bool subtract_s16(uint16_t a, uint16_t b, uint16_t expected)
{
    mcs51_t proc = {.C = {0xc3, 0xec, 0x9e, 0xf5, 0x20, 0xed, 0x9f, 0xf5, 0x21}};
    mcs51_init(&proc);

    proc.D[0x05] = a >> 8;
    proc.D[0x04] = a;

    proc.D[0x07] = b >> 8;
    proc.D[0x06] = b;

    RUN_UNTIL_NOP();

    return proc.D[0x21] == ((expected >> 8) & 0xFF) && proc.D[0x20] == ((expected) &0xFF);
}

TEST(test_subtract_s16)
{
    return subtract_s16(0xCDAB, 0x35DD, 0x97CE)
           && subtract_s16(0x1234, 0x1122, 0x0112)
           && subtract_s16(0, 1, UINT16_MAX)
           && subtract_s16(1, 3, UINT16_MAX - 1);
}

TEST(test_accumulator)
{
    mcs51_t proc = {.C = {0xf5, 0x30}}; // MOV 0x30, A
    mcs51_init(&proc);

    proc.D[SFR_ACC] = 0xDE;

    RUN_UNTIL_NOP();

    return proc.D[0x30] == 0xDE;
}

TEST(test_sfr_sbuf)
{
    mcs51_t proc = {.C = {0xf5, SFR_SBUF}}; // MOV SBUF, A
    mcs51_init(&proc);

    proc.D[SFR_ACC] = 0xDE;

    RUN_UNTIL_NOP();

    return proc._sfr_dirty_sbuf == true;
}

/**
 * MOV R0, #0x80
 * MOV @R0, #0xAB
 */
TEST(test_indirect_addressing)
{
    mcs51_t proc = {.C = {0x78, 0x80, 0x76, 0xab}};
    mcs51_init(&proc);

    proc.D[0x80] = 0xFF;

    RUN_UNTIL_NOP();

    return proc.D[0x80] == 0xFF && proc.D[0x80 + 0x80] == 0xAB;
}

/**
 * MOV TMOD, #0x01 ; Set ET0 to 16-bit mode
 * SETB TR0
 * PUSH TL0 ; (6x)
 */
TEST(test_timer_0)
{
    mcs51_t proc = {.C = {0x75, 0x89, 0x01, 0xd2, 0x8c, 0xc0, 0x8a, 0xc0, 0x8a, 0xc0, 0x8a, 0xc0, 0x8a, 0xc0, 0x8a, 0xc0, 0x8a}};
    mcs51_init(&proc);

    RUN_UNTIL_NOP();

    return proc.D[0x07] == 0
           && proc.D[0x08] == 1
           && proc.D[0x09] == 3
           && proc.D[0x0a] == 5
           && proc.D[0x0b] == 7
           && proc.D[0x0c] == 9
           && proc.D[0x0d] == 11
           && proc.D[0x0e] == 0;
}

/**
 * .ORG 0000h
 *     SJMP main
 *
 * .ORG 000Bh
 *     MOV R1, #0xDE
 *     RETI
 *
 * main:
 *     SETB EA
 *     SETB ET0
 *     MOV TMOD, #0x01 ; Set ET0 to 16-bit mode
 *     MOV TH0, #0xFF
 *     MOV TL0, #0xF8
 *     NOP
 *
 *     SETB TR0 ; S6P2: TL0 0xF9
 *     ; S6P2: TL0 0xFa
 *     ; S6P2: TL0 0xFb
 *     ; S6P2: TL0 0xFc
 *     ; S6P2: TL0 0xFd
 *     ; S6P2: TL0 0xFe
 *     ; S6P2: TL0 0xFf
 *     ; S6P2: TL0 0x00
 *     ; S5P2: NVIC samples flag ; S6P2: TL0 0x01
 *     ; S1P2: NVIC LJMP Cycle 1
 *     ; S1P2: NVIC LJMP Cycle 2
 *     ; S1P2: First ISR instruction
 */
TEST(test_timer_0_isr)
{
    bool success = true;

    mcs51_t proc = {.C = {0x80, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0xde, 0x32, 0xd2, 0xaf, 0xd2, 0xa9, 0x75, 0x89, 0x01, 0x75, 0x8c, 0xff, 0x75, 0x8a, 0xf8, 0x00, 0xd2, 0x8c}};
    mcs51_init(&proc);

    RUN_UNTIL_NOP();

    MACHINE_CYCLE(); // SETB TR0

    for (int i = 0; i < 8; i++)
    {
        MACHINE_CYCLE();
        success &= proc.D[0x01] == 0x00;
        success &= proc._nvic._isr_active_msk == 0;
    }

    // ISR flags are sampled, the next instruction will be HW generated (NVIC)

    // LJMP takes 2 cycles
    for (int i = 0; i < 2; i++)
    {
        MACHINE_CYCLE();
        success &= proc.D[0x01] == 0x00;
        success &= proc._nvic._isr_active_msk != 0;
    }

    MACHINE_CYCLE(); // MOV of the ISR

    success &= proc.D[0x01] == 0xDE;

    return success;
}

TEST(test_sfr_addresses)
{
    bool success = true;

    mcs51_t proc = {};
    mcs51_init(&proc);

    const int elem_count = sizeof(proc.sfr_map) / sizeof(sfr_t);
    success &= (elem_count == 256);

    for (int i = 0; i < elem_count; i++)
        success &= proc.sfr_map[i].address == i;

    return success;
}

TEST(test_sfr_names)
{
    bool success = true;

    mcs51_t proc = {};
    mcs51_init(&proc);

    success &= strcmp(proc.sfr_map[SFR_ACC].name, "ACC") == 0;
    success &= strcmp(proc.sfr_map[SFR_IE].name, "IE") == 0;

    return success;
}

/**
 * .ORG 0000h
 *      LJMP main
 *
 * .ORG 0003h
 *     MOV @R0, #0xDE
 *     INC R0
 *     RETI
 *
 * .ORG 000Bh
 *     MOV @R0, #0xAD
 *     INC R0
 *     RETI
 *
 * .ORG 0013h
 *     MOV @R0, #0xBE
 *     INC R0
 *     RETI
 *
 * .ORG 001Bh
 *     MOV @R0, #0xEF
 *     INC R0
 *     RETI
 *
 * main:
 *     MOV R0, #0x30
 *     MOV IE, #0b10001111 ; Enable EA, ET1, EX1, ET0, EX0
 */
TEST(test_isr_nesting)
{
    mcs51_t proc = {.C = {0x02, 0x00, 0x1f, 0x76, 0xde, 0x08, 0x32, 0x00, 0x00, 0x00, 0x00, 0x76, 0xad, 0x08, 0x32, 0x00, 0x00, 0x00, 0x00, 0x76, 0xbe, 0x08, 0x32, 0x00, 0x00, 0x00, 0x00, 0x76, 0xef, 0x08, 0x32, 0x78, 0x30, 0x75, 0xa8, 0x8f, 0x00}};
    mcs51_init(&proc);

    RUN_UNTIL_NOP();

    proc.D[SFR_TCON] |= SFR_TCON_TF1_Msk;

    MACHINE_CYCLE();
    MACHINE_CYCLE();

    proc.D[SFR_TCON] |= SFR_TCON_IE1_Msk;

    MACHINE_CYCLE();
    MACHINE_CYCLE();

    proc.D[SFR_TCON] |= SFR_TCON_TF0_Msk;

    MACHINE_CYCLE();
    MACHINE_CYCLE();

    proc.D[SFR_TCON] |= SFR_TCON_IE0_Msk;

    RUN_UNTIL_NOP();

    return proc.D[0x30] == 0xDE
           && proc.D[0x31] == 0xAD
           && proc.D[0x32] == 0xBE
           && proc.D[0x33] == 0xEF;
}

/**
 * .ORG 0000h
 *     LJMP main
 *
 * .ORG 0003h
 *     MOV R0, #0xAB
 *     RETI
 *
 * main:
 *     MOV IE, #0b10000001 ; Enable EA, EX0
 *     NOP
 *     MOV IP, #0
 *     MUL AB
 */
TEST(test_max_interrupt_latency)
{
    bool success = true;

    mcs51_t proc = {.C = {0x02, 0x00, 0x06, 0x78, 0xab, 0x32, 0x75, 0xa8, 0x81, 0x00, 0x75, 0xb8, 0x00, 0xa4}};
    mcs51_init(&proc);

    RUN_UNTIL_NOP();

    proc.D[SFR_TCON] |= SFR_TCON_IE0_Msk;

    MACHINE_CYCLE(); // MOV IP, #0 and sample of NVIC flags
    MACHINE_CYCLE(); // MOV IP, #0
    // MOV IP, #0 had/has IP access, thus no interrupt

    // 4 cycles of MUL
    MACHINE_CYCLE();
    MACHINE_CYCLE();
    MACHINE_CYCLE();
    MACHINE_CYCLE();

    MACHINE_CYCLE(); // LJMP
    MACHINE_CYCLE(); // LJMP

    success &= proc.D[0x00] == 0x00;

    MACHINE_CYCLE(); // MOV

    success &= proc.D[0x00] == 0xAB;

    return success;
}

int main(int argc, char* argv[])
{
    int code = 0;

    if (argc > 1 && argv[1] && strcmp(argv[1], "--verbose") == 0)
        s_test_cfg.verbose = true;

    RUN_TEST(test_nop);
    RUN_TEST(test_data_xdata_exchange);
    RUN_TEST(test_swap);
    RUN_TEST(test_swap2);
    RUN_TEST(test_subtract_s16);
    RUN_TEST(test_accumulator);
    RUN_TEST(test_sfr_sbuf);
    RUN_TEST(test_indirect_addressing);
    RUN_TEST(test_timer_0);
    RUN_TEST(test_timer_0_isr);
    RUN_TEST(test_sfr_addresses);
    RUN_TEST(test_sfr_names);
    RUN_TEST(test_isr_nesting);
    RUN_TEST(test_max_interrupt_latency);

    return code;
}

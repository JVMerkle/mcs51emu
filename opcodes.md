# MCS51 Opcodes

Source: https://www.keil.com/support/man/docs/is51/is51_opcodes.htm

Many instructions have required args that are described in the following listing:

- addr11 An 11-bit address destination. This argument is used by ACALL and AJMP instructions.
  The target of the CALL or JMP must lie within the same 2K page as the first byte of the following instruction.
- addr16 A 16-bit address destination. This argument is used by LCALL and LJMP instructions.
- bit A direct addressed bit in internal data RAM or SFR memory.
- direct An internal data RAM location (0-127) or SFR (128-255).
- immediate A constant included in the instruction encoding.
- offset A signed (two's complement) 8-bit offset (-128 to 127) relative to the first byte of the following instruction.
- @Ri An internal data RAM location (0-255) addressed indirectly through R0 or R1.
- Rn Register R0-R7.

| Code | Bytes | Cycles | Mnemonic | Operand 1 | Operand 2 | Operand 3 |
|------|-------|--------|----------|-----------|-----------|-----------|
| 00   | 1     | 1      | NOP      |           |           |           |
| 01   | 2     | 2      | AJMP     | addr11    |           |           |
| 02   | 3     | 2      | LJMP     | addr16    |           |           |
| 03   | 1     | 1      | RR       | A         |           |           |
| 04   | 1     | 1      | INC      | A         |           |           |
| 05   | 2     | 1      | INC      | direct    |           |           |
| 06   | 1     | 1      | INC      | @R0       |           |           |
| 07   | 1     | 1      | INC      | @R1       |           |           |
| 08   | 1     | 1      | INC      | R0        |           |           |
| 09   | 1     | 1      | INC      | R1        |           |           |
| 0A   | 1     | 1      | INC      | R2        |           |           |
| 0B   | 1     | 1      | INC      | R3        |           |           |
| 0C   | 1     | 1      | INC      | R4        |           |           |
| 0D   | 1     | 1      | INC      | R5        |           |           |
| 0E   | 1     | 1      | INC      | R6        |           |           |
| 0F   | 1     | 1      | INC      | R7        |           |           |
| 10   | 3     | 2      | JBC      | bit       | offset    |           |
| 11   | 2     | 2      | ACALL    | addr11    |           |           |
| 12   | 3     | 2      | LCALL    | addr16    |           |           |
| 13   | 1     | 1      | RRC      | A         |           |           |
| 14   | 1     | 1      | DEC      | A         |           |           |
| 15   | 2     | 1      | DEC      | direct    |           |           |
| 16   | 1     | 1      | DEC      | @R0       |           |           |
| 17   | 1     | 1      | DEC      | @R1       |           |           |
| 18   | 1     | 1      | DEC      | R0        |           |           |
| 19   | 1     | 1      | DEC      | R1        |           |           |
| 1A   | 1     | 1      | DEC      | R2        |           |           |
| 1B   | 1     | 1      | DEC      | R3        |           |           |
| 1C   | 1     | 1      | DEC      | R4        |           |           |
| 1D   | 1     | 1      | DEC      | R5        |           |           |
| 1E   | 1     | 1      | DEC      | R6        |           |           |
| 1F   | 1     | 1      | DEC      | R7        |           |           |
| 20   | 3     | 2      | JB       | bit       | offset    |           |
| 21   | 2     | 2      | AJMP     | addr11    |           |           |
| 22   | 1     | 2      | RET      |           |           |           |
| 23   | 1     | 1      | RL       | A         |           |           |
| 24   | 2     | 1      | ADD      | A         | #immed    |           |
| 25   | 2     | 1      | ADD      | A         | direct    |           |
| 26   | 1     | 1      | ADD      | A         | @R0       |           |
| 27   | 1     | 1      | ADD      | A         | @R1       |           |
| 28   | 1     | 1      | ADD      | A         | R0        |           |
| 29   | 1     | 1      | ADD      | A         | R1        |           |
| 2A   | 1     | 1      | ADD      | A         | R2        |           |
| 2B   | 1     | 1      | ADD      | A         | R3        |           |
| 2C   | 1     | 1      | ADD      | A         | R4        |           |
| 2D   | 1     | 1      | ADD      | A         | R5        |           |
| 2E   | 1     | 1      | ADD      | A         | R6        |           |
| 2F   | 1     | 1      | ADD      | A         | R7        |           |
| 30   | 3     | 2      | JNB      | bit       | offset    |           |
| 31   | 2     | 2      | ACALL    | addr11    |           |           |
| 32   | 1     | 2      | RETI     |           |           |           |
| 33   | 1     | 1      | RLC      | A         |           |           |
| 34   | 2     | 1      | ADDC     | A         | #immed    |           |
| 35   | 2     | 1      | ADDC     | A         | direct    |           |
| 36   | 1     | 1      | ADDC     | A         | @R0       |           |
| 37   | 1     | 1      | ADDC     | A         | @R1       |           |
| 38   | 1     | 1      | ADDC     | A         | R0        |           |
| 39   | 1     | 1      | ADDC     | A         | R1        |           |
| 3A   | 1     | 1      | ADDC     | A         | R2        |           |
| 3B   | 1     | 1      | ADDC     | A         | R3        |           |
| 3C   | 1     | 1      | ADDC     | A         | R4        |           |
| 3D   | 1     | 1      | ADDC     | A         | R5        |           |
| 3E   | 1     | 1      | ADDC     | A         | R6        |           |
| 3F   | 1     | 1      | ADDC     | A         | R7        |           |
| 40   | 2     | 2      | JC       | offset    |           |           |
| 41   | 2     | 2      | AJMP     | addr11    |           |           |
| 42   | 2     | 1      | ORL      | direct    | A         |           |
| 43   | 3     | 2      | ORL      | direct    | #immed    |           |
| 44   | 2     | 1      | ORL      | A         | #immed    |           |
| 45   | 2     | 1      | ORL      | A         | direct    |           |
| 46   | 1     | 1      | ORL      | A         | @R0       |           |
| 47   | 1     | 1      | ORL      | A         | @R1       |           |
| 48   | 1     | 1      | ORL      | A         | R0        |           |
| 49   | 1     | 1      | ORL      | A         | R1        |           |
| 4A   | 1     | 1      | ORL      | A         | R2        |           |
| 4B   | 1     | 1      | ORL      | A         | R3        |           |
| 4C   | 1     | 1      | ORL      | A         | R4        |           | 
| 4D   | 1     | 1      | ORL      | A         | R5        |           |
| 4E   | 1     | 1      | ORL      | A         | R6        |           |
| 4F   | 1     | 1      | ORL      | A         | R7        |           |
| 50   | 2     | 2      | JNC      | offset    |           |           |
| 51   | 2     | 2      | ACALL    | addr11    |           |           |
| 52   | 2     | 1      | ANL      | direct    | A         |           |
| 53   | 3     | 2      | ANL      | direct    | #immed    |           |
| 54   | 2     | 1      | ANL      | A         | #immed    |           |
| 55   | 2     | 1      | ANL      | A         | direct    |           |
| 56   | 1     | 1      | ANL      | A         | @R0       |           |
| 57   | 1     | 1      | ANL      | A         | @R1       |           |
| 58   | 1     | 1      | ANL      | A         | R0        |           |
| 59   | 1     | 1      | ANL      | A         | R1        |           |
| 5A   | 1     | 1      | ANL      | A         | R2        |           |
| 5B   | 1     | 1      | ANL      | A         | R3        |           |
| 5C   | 1     | 1      | ANL      | A         | R4        |           |
| 5D   | 1     | 1      | ANL      | A         | R5        |           |
| 5E   | 1     | 1      | ANL      | A         | R6        |           |
| 5F   | 1     | 1      | ANL      | A         | R7        |           | 
| 60   | 2     | 2      | JZ       | offset    |           |           |
| 61   | 2     | 2      | AJMP     | addr11    |           |           |
| 62   | 2     | 1      | XRL      | direct    | A         |           |
| 63   | 3     | 2      | XRL      | direct    | #immed    |           |
| 64   | 2     | 1      | XRL      | A         | #immed    |           |
| 65   | 2     | 1      | XRL      | A         | direct    |           |
| 66   | 1     | 1      | XRL      | A         | @R0       |           |
| 67   | 1     | 1      | XRL      | A         | @R1       |           |
| 68   | 1     | 1      | XRL      | A         | R0        |           |
| 69   | 1     | 1      | XRL      | A         | R1        |           |
| 6A   | 1     | 1      | XRL      | A         | R2        |           |
| 6B   | 1     | 1      | XRL      | A         | R3        |           |
| 6C   | 1     | 1      | XRL      | A         | R4        |           |
| 6D   | 1     | 1      | XRL      | A         | R5        |           |
| 6E   | 1     | 1      | XRL      | A         | R6        |           |
| 6F   | 1     | 1      | XRL      | A         | R7        |           |
| 70   | 2     | 2      | JNZ      | offset    |           |           |
| 71   | 2     | 2      | ACALL    | addr11    |           |           |
| 72   | 2     | 2      | ORL      | C         | bit       |           |
| 73   | 1     | 2      | JMP      | @A+DPTR   |           |           |
| 74   | 2     | 1      | MOV      | A         | #immed    |           |
| 75   | 3     | 2      | MOV      | direct    | #immed    |           |
| 76   | 2     | 1      | MOV      | @R0       | #immed    |           |
| 77   | 2     | 1      | MOV      | @R1       | #immed    |           |
| 78   | 2     | 1      | MOV      | R0        | #immed    |           |
| 79   | 2     | 1      | MOV      | R1        | #immed    |           |
| 7A   | 2     | 1      | MOV      | R2        | #immed    |           |
| 7B   | 2     | 1      | MOV      | R3        | #immed    |           |
| 7C   | 2     | 1      | MOV      | R4        | #immed    |           |
| 7D   | 2     | 1      | MOV      | R5        | #immed    |           |
| 7E   | 2     | 1      | MOV      | R6        | #immed    |           |
| 7F   | 2     | 1      | MOV      | R7        | #immed    |           |
| 80   | 2     | 2      | SJMP     | offset    |           |           |
| 81   | 2     | 2      | AJMP     | addr11    |           |           |
| 82   | 2     | 2      | ANL      | C         | bit       |           |
| 83   | 1     | 2      | MOVC     | A         | @A+PC     |           |
| 84   | 1     | 4      | DIV      | AB        |           |           |
| 85   | 3     | 2      | MOV      | direct    | direct    |           |
| 86   | 2     | 2      | MOV      | direct    | @R0       |           |
| 87   | 2     | 2      | MOV      | direct    | @R1       |           |
| 88   | 2     | 2      | MOV      | direct    | R0        |           |
| 89   | 2     | 2      | MOV      | direct    | R1        |           |
| 8A   | 2     | 2      | MOV      | direct    | R2        |           |
| 8B   | 2     | 2      | MOV      | direct    | R3        |           |
| 8C   | 2     | 2      | MOV      | direct    | R4        |           |
| 8D   | 2     | 2      | MOV      | direct    | R5        |           |
| 8E   | 2     | 2      | MOV      | direct    | R6        |           |
| 8F   | 2     | 2      | MOV      | direct    | R7        |           |
| 90   | 3     | 2      | MOV      | DPTR      | #immed    |           |
| 91   | 2     | 2      | ACALL    | addr11    |           |           |
| 92   | 2     | 2      | MOV      | bit       | C         |           |
| 93   | 1     | 2      | MOVC     | A         | @A+DPTR   |           |
| 94   | 2     | 1      | SUBB     | A         | #immed    |           |
| 95   | 2     | 1      | SUBB     | A         | direct    |           |
| 96   | 1     | 1      | SUBB     | A         | @R0       |           |
| 97   | 1     | 1      | SUBB     | A         | @R1       |           |
| 98   | 1     | 1      | SUBB     | A         | R0        |           |
| 99   | 1     | 1      | SUBB     | A         | R1        |           |
| 9A   | 1     | 1      | SUBB     | A         | R2        |           |
| 9B   | 1     | 1      | SUBB     | A         | R3        |           |
| 9C   | 1     | 1      | SUBB     | A         | R4        |           |
| 9D   | 1     | 1      | SUBB     | A         | R5        |           |
| 9E   | 1     | 1      | SUBB     | A         | R6        |           |
| 9F   | 1     | 1      | SUBB     | A         | R7        |           |
| A0   | 2     | 2      | ORL      | C         | /bit      |           |
| A1   | 2     | 2      | AJMP     | addr11    |           |           |
| A2   | 2     | 1      | MOV      | C         | bit       |           |
| A3   | 1     | 2      | INC      | DPTR      |           |           |
| A4   | 1     | 4      | MUL      | AB        |           |           |
| A5   |       |        | reserved |           |           |           |
| A6   | 2     | 2      | MOV      | @R0       | direct    |           |
| A7   | 2     | 2      | MOV      | @R1       | direct    |           |
| A8   | 2     | 2      | MOV      | R0        | direct    |           |
| A9   | 2     | 2      | MOV      | R1        | direct    |           |
| AA   | 2     | 2      | MOV      | R2        | direct    |           |
| AB   | 2     | 2      | MOV      | R3        | direct    |           |
| AC   | 2     | 2      | MOV      | R4        | direct    |           |
| AD   | 2     | 2      | MOV      | R5        | direct    |           |
| AE   | 2     | 2      | MOV      | R6        | direct    |           |
| AF   | 2     | 2      | MOV      | R7        | direct    |           |
| B0   | 2     | 2      | ANL      | C         | /bit      |           |
| B1   | 2     | 2      | ACALL    | addr11    |           |           |
| B2   | 2     | 1      | CPL      | bit       |           |           |
| B3   | 1     | 1      | CPL      | C         |           |           |
| B4   | 3     | 2      | CJNE     | A         | #immed    | offset    |
| B5   | 3     | 2      | CJNE     | A         | direct    | offset    |
| B6   | 3     | 2      | CJNE     | @R0       | #immed    | offset    |
| B7   | 3     | 2      | CJNE     | @R1       | #immed    | offset    |
| B8   | 3     | 2      | CJNE     | R0        | #immed    | offset    |
| B9   | 3     | 2      | CJNE     | R1        | #immed    | offset    |
| BA   | 3     | 2      | CJNE     | R2        | #immed    | offset    |
| BB   | 3     | 2      | CJNE     | R3        | #immed    | offset    |
| BC   | 3     | 2      | CJNE     | R4        | #immed    | offset    |
| BD   | 3     | 2      | CJNE     | R5        | #immed    | offset    |
| BE   | 3     | 2      | CJNE     | R6        | #immed    | offset    |
| BF   | 3     | 2      | CJNE     | R7        | #immed    | offset    |
| C0   | 2     | 2      | PUSH     | direct    |           |           |
| C1   | 2     | 2      | AJMP     | addr11    |           |           |
| C2   | 2     | 1      | CLR      | bit       |           |           |
| C3   | 1     | 1      | CLR      | C         |           |           |
| C4   | 1     | 1      | SWAP     | A         |           |           |
| C5   | 2     | 1      | XCH      | A         | direct    |           |
| C6   | 1     | 1      | XCH      | A         | @R0       |           |
| C7   | 1     | 1      | XCH      | A         | @R1       |           |
| C8   | 1     | 1      | XCH      | A         | R0        |           |
| C9   | 1     | 1      | XCH      | A         | R1        |           |
| CA   | 1     | 1      | XCH      | A         | R2        |           |
| CB   | 1     | 1      | XCH      | A         | R3        |           |
| CC   | 1     | 1      | XCH      | A         | R4        |           |
| CD   | 1     | 1      | XCH      | A         | R5        |           |
| CE   | 1     | 1      | XCH      | A         | R6        |           |
| CF   | 1     | 1      | XCH      | A         | R7        |           |
| D0   | 2     | 2      | POP      | direct    |           |           |
| D1   | 2     | 2      | ACALL    | addr11    |           |           |
| D2   | 2     | 1      | SETB     | bit       |           |           |
| D3   | 1     | 1      | SETB     | C         |           |           |
| D4   | 1     | 1      | DA       | A         |           |           |
| D5   | 3     | 2      | DJNZ     | direct    | offset    |           |
| D6   | 1     | 1      | XCHD     | A         | @R0       |           |
| D7   | 1     | 1      | XCHD     | A         | @R1       |           |
| D8   | 2     | 2      | DJNZ     | R0        | offset    |           |
| D9   | 2     | 2      | DJNZ     | R1        | offset    |           |
| DA   | 2     | 2      | DJNZ     | R2        | offset    |           |
| DB   | 2     | 2      | DJNZ     | R3        | offset    |           |
| DC   | 2     | 2      | DJNZ     | R4        | offset    |           |
| DD   | 2     | 2      | DJNZ     | R5        | offset    |           |
| DE   | 2     | 2      | DJNZ     | R6        | offset    |           |
| DF   | 2     | 2      | DJNZ     | R7        | offset    |           |
| E0   | 1     | 2      | MOVX     | A         | @DPTR     |           |
| E1   | 2     | 2      | AJMP     | addr11    |           |           |
| E2   | 1     | 2      | MOVX     | A         | @R0       |           |
| E3   | 1     | 2      | MOVX     | A         | @R1       |           |
| E4   | 1     | 1      | CLR      | A         |           |           |
| E5   | 2     | 1      | MOV      | A         | direct    |           |
| E6   | 1     | 1      | MOV      | A         | @R0       |           |
| E7   | 1     | 1      | MOV      | A         | @R1       |           |
| E8   | 1     | 1      | MOV      | A         | R0        |           |
| E9   | 1     | 1      | MOV      | A         | R1        |           |
| EA   | 1     | 1      | MOV      | A         | R2        |           |
| EB   | 1     | 1      | MOV      | A         | R3        |           |
| EC   | 1     | 1      | MOV      | A         | R4        |           |
| ED   | 1     | 1      | MOV      | A         | R5        |           |
| EE   | 1     | 1      | MOV      | A         | R6        |           |
| EF   | 1     | 1      | MOV      | A         | R7        |           |
| F0   | 1     | 2      | MOVX     | @DPTR     | A         |           |
| F1   | 2     | 2      | ACALL    | addr11    |           |           |
| F2   | 1     | 2      | MOVX     | @R0       | A         |           |
| F3   | 1     | 2      | MOVX     | @R1       | A         |           |
| F4   | 1     | 1      | CPL      | A         |           |           |
| F5   | 2     | 1      | MOV      | direct    | A         |           |
| F6   | 1     | 1      | MOV      | @R0       | A         |           |
| F7   | 1     | 1      | MOV      | @R1       | A         |           |
| F8   | 1     | 1      | MOV      | R0        | A         |           |
| F9   | 1     | 1      | MOV      | R1        | A         |           |
| FA   | 1     | 1      | MOV      | R2        | A         |           |
| FB   | 1     | 1      | MOV      | R3        | A         |           |
| FC   | 1     | 1      | MOV      | R4        | A         |           |
| FD   | 1     | 1      | MOV      | R5        | A         |           |
| FE   | 1     | 1      | MOV      | R6        | A         |           |
| FF   | 1     | 1      | MOV      | R7        | A         |           |

# MCS51 (8051) Emulator Library

This is a feature incomplete, platform independent implementation of an MCS51 microcontroller for educational purposes.

The emulator supports stepwise clocking through all 12 MCU states S1P1 - S6P2 (that's one machine cycle).

- [Opcodes](./opcodes.md)
- [Special Function Registers](./sfrs.md)

## Requirements

- CMake 3.16 or later
- python3 (code generation)

## Features

- [X] S1P1 - S6P2 clocking
- [X] Memory mapping for directly and indirectly addressed RAM
- [X] Functional interrupt system
- [X] SFR hook support
- [X] Register bank switching
- [X] Timer 0 Mode 0 and Mode 1 support
- [X] Timer 1 Mode 2 support
- [X] Serial mode 1 TX support (8-bit_mask)
- [X] Basic test suite
- [X] Interrupt priorities
- [ ] External code mapping
- [ ] All timer modes implemented
- [ ] All opcodes implemented
- [ ] All SFR functionalities implemented

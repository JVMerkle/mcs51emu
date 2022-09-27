#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
# SPDX-License-Identifier: MIT
#

import sys


class CMacro:
    def __init__(self, name: str = '', value: str = '', comment=''):
        self.name = name
        self.value = value
        self.comment = comment

    def empty(self) -> bool:
        return len(self.name) == 0 or len(self.value) == 0

    def print(self, file=sys.stdout):
        if not self.empty():
            value = '(%s)' % self.value
            if len(self.comment) == 0:
                print('#define %-40s %-40s' % (self.name, value), file=file)
            else:
                print('#define %-40s %-40s /// %s' % (self.name, value, self.comment), file=file)


class SFRBit:
    def __init__(self, name, description):
        self.name = name
        self.description = description

    def valid(self) -> bool:
        return len(self.name) > 0


class SFR:
    def __init__(self, address: str, name: str, bit_addressable: bool, bits=None):
        if bits is None:
            bits = []
        self.address = address
        self.name = name
        self.bit_addressable = bit_addressable
        self.bits = bits

        contains_at_least_one_named_bit = False
        for b in bits:
            if len(b.name) != 0:
                contains_at_least_one_named_bit = True
                break

        # Add numbered bits if no bits are given
        if not contains_at_least_one_named_bit:
            for i, b in enumerate(bits):
                if len(b.name) == 0:
                    b.name = str(i)

    def add_bit(self, bit):
        self.bits.append(bit)

    def macro_prefix(self):
        return 'SFR_%s' % (self.name)

    def byte_macro(self) -> CMacro:
        macro_value = '0x%s' % self.address
        macro_comment = ''
        if self.bit_addressable == 'Yes':
            macro_comment = 'Bit addressable'

        return CMacro(self.macro_prefix(), macro_value, macro_comment)

    def bit_position_macro(self, index) -> CMacro:
        bit = self.bits[index]
        if bit.valid():
            name = '%s_%s_Pos' % (self.macro_prefix(), bit.name)
            return CMacro(name, '%dU' % index, bit.description)
        else:
            return CMacro()

    def bit_mask_macro(self, index) -> CMacro:
        bit = self.bits[index]
        if bit.valid():
            name = '%s_%s_Msk' % (self.macro_prefix(), bit.name)
            return CMacro(name, '1U << %s' % self.bit_position_macro(index).name, bit.description)
        else:
            return CMacro()

    def print_macro_definitions(self, file=sys.stdout):
        self.byte_macro().print(file)
        for i, bit in enumerate(self.bits):
            self.bit_position_macro(i).print(file)
            self.bit_mask_macro(i).print(file)


sfr_dict = {}
with open(sys.argv[1], 'r') as file:
    line_no = 0
    table_line_no = 0
    while True:
        line = file.readline()
        if len(line) == 0:
            break

        line_no += 1

        # Parse table entries only
        if not line.lstrip().startswith("|"):
            continue

        table_line_no += 1

        # Table header
        if table_line_no < 3:
            continue

        cells = line.strip().strip('|').strip().split('|')

        if len(cells) != 11:
            print('Bad table entry in line %d: 11 cells expected, got %d' % (line_no, len(cells)), file=sys.stderr)
            continue

        cells = [s.strip() for s in cells]

        bits = []
        for bit in cells[3:]:
            name = bit.split(' ')[0]
            description = ' '.join(bit.split(' ')[1:]).strip()
            bits.append(SFRBit(name, description))

        bits.reverse()

        bit_addressable = False
        if cells[2] == 'X':
            bit_addressable = True
        elif len(cells[2]) == 0:
            bit_addressable = False
        else:
            print('Bad table entry in line %d: Could not parse cell bit-addressable: %s' % (line_no, cells[2]), file=sys.stderr)
            continue

        if cells[0] not in sfr_dict:
            sfr_dict[cells[0]] = SFR(cells[0], cells[1], bit_addressable, bits)
        else:
            print('Duplicate table entry in line %d' % line_no, file=sys.stderr)
            sys.exit(1)

file_header = """/**
 * Generated file, DO NOT MODIFY!
 * SPDX-License-Identifier: MIT
 */
"""

with open('sfr_definitions_gen.h', 'w') as out:
    print(file_header, file=out)
    print('#pragma once', file=out)
    print('', file=out)
    CMacro('SFR_BASE_ADDRESS', '0x80').print(file=out)
    print('', file=out)
    for k, sfr in sfr_dict.items():
        sfr.print_macro_definitions(file=out)
        print('', file=out)

with open('sfr_map_gen.h', 'w') as out:
    print(file_header, file=out)
    print('#pragma once', file=out)
    print('', file=out)
    print('#include "sfr.h"', file=out)
    print('', file=out)
    print('#define SFR_MAP_SIZE (0x100)', file=out)
    print('extern const sfr_t sfr_map[SFR_MAP_SIZE];', file=out)

with open('sfr_map_gen.c', 'w') as out:
    print(file_header, file=out)
    print('#include "sfr_definitions_gen.h"', file=out)
    print('#include "sfr_map_gen.h"', file=out)
    print('', file=out)

    print('const sfr_t sfr_map[SFR_MAP_SIZE] = {', file=out)
    for k, sfr in sfr_dict.items():
        c_boolean = 'false'
        if sfr.bit_addressable:
            c_boolean = 'true'
        print('    [SFR_%s] = {.name = "%s", .bit_addressable = %s},' % (sfr.name, sfr.name, c_boolean), file=out)
    print('};', file=out)

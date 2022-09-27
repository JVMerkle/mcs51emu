#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: 2022 Julian Merkle <info@jvmerkle.de>
# SPDX-License-Identifier: MIT
#

import sys


class Opcode:
    def __init__(self, code, size_bytes, cycles, mnemonic, args):
        self.code = code
        self.size_bytes = size_bytes
        self.cycles = cycles
        self.mnemonic = mnemonic
        self.args = args

    def get_actor_function_name(self):
        name = self.mnemonic
        for arg in self.args:
            if len(arg) > 0:
                arg = arg.replace('@', 'At').replace('/', 'Neg').replace('#', '').replace('+', 'Plus')
                name += '_' + arg
        return name

    def get_actor_signature(self):
        return 'void %s(mcs51_t*);' % self.get_actor_function_name()

    def get_actor_weak_implementation(self):
        impl = 'void __attribute__((weak)) %s(mcs51_t* p) {\n' % self.get_actor_function_name()
        impl += '    fprintf(stderr, "Opcode not implemented: %s\\n");\n' % self.get_actor_function_name()
        impl += '    if(p->_abort_on_unimplemented_opcode)\n'
        impl += '        abort();\n'
        return impl + '}'

    def as_c_struct_initializer(self) -> str:
        initializer = '{ .code = %s, .bytes = %s, .cycles = %s, .mnemonic = "%s", .actor = &%s' \
                      % (self.code, self.size_bytes, self.cycles, self.mnemonic, self.get_actor_function_name())

        # Fill to 3 arguments
        args = self.args
        while len(args) < 3:
            args.append('')

        for i, arg in enumerate(args):
            initializer += ', .arg' + str(i + 1) + ' = "%s"' % arg

        initializer += '}'
        return initializer


opcode_dict = {}
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

        if len(cells) != 7:
            print('Bad table entry in line %d: 7 cells expected, got %d' % (line_no, len(cells)), file=sys.stderr)
            continue

        cells = [s.strip() for s in cells]

        if cells[3] == 'reserved':
            cells = [cells[0], '0', '0', 'RESERVED']

        code_hex = '0x' + cells[0]

        args = []
        for arg in cells[4:]:
            args.append(arg.rstrip(','))

        if code_hex not in opcode_dict:
            opcode_dict[code_hex] = Opcode(code_hex, cells[1], cells[2], cells[3], args)
        else:
            print('Duplicate table entry in line %d for opcode %s' % (line_no, code_hex), file=sys.stderr)
            sys.exit(1)

# 256 opcodes!
if len(opcode_dict) != 256:
    print('Not all 256 opcodes are registered', file=sys.stderr)
    sys.exit(1)

for i, (k, opcode) in enumerate(opcode_dict.items()):
    if i != int(opcode.code, 16):
        print('Opcode hex does not match registration order: %s' % opcode.code, file=sys.stderr)
        sys.exit(1)

# Map with unique opcode function signatures (e.g. there are multiple ACALL's)
signature_opcode_map = {}
for k, opcode in opcode_dict.items():
    signature_opcode_map[opcode.get_actor_signature()] = opcode

file_header = """/**
 * Generated file, DO NOT MODIFY!
 * SPDX-License-Identifier: MIT
 */
"""

with open('opcode_map_gen.h', 'w') as out:
    print(file_header, file=out)
    print('#pragma once', file=out)
    print('', file=out)
    print('#include "opcode.h"', file=out)
    print('', file=out)
    print('#define OPCODE_MAP_SIZE (0x100)', file=out)
    print('extern const opcode_t opcode_map[OPCODE_MAP_SIZE];', file=out)

with open('opcode_map_gen.c', 'w') as out:
    print(file_header, file=out)
    print('#include "opcode.h"', file=out)
    print('#include "opcode_map_gen.h"', file=out)
    print('#include "opcode_impl_gen.h"', file=out)
    print('', file=out)
    print('const opcode_t opcode_map[OPCODE_MAP_SIZE] = {', file=out)
    for k, opcode in opcode_dict.items():
        print('    [%s] = %s,' % (opcode.code, opcode.as_c_struct_initializer()), file=out)
    print('};', file=out)

with open('opcode_impl_gen.h', 'w') as out:
    print(file_header, file=out)
    print('#pragma once', file=out)
    print('', file=out)
    print('typedef struct mcs51_t mcs51_t;', file=out)
    print('', file=out)

    for k, opcode in signature_opcode_map.items():
        print(opcode.get_actor_signature(), file=out)

with open('opcode_impl_weak_gen.c', 'w') as out:
    print(file_header, file=out)
    print('#include <stdlib.h>', file=out)
    print('#include <stdio.h>', file=out)
    print('#include "mcs51.h"', file=out)
    print('', file=out)

    for k, opcode in signature_opcode_map.items():
        print(opcode.get_actor_weak_implementation(), file=out)
        print('', file=out)

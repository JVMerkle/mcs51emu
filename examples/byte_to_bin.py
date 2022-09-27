#!/usr/bin/env python3
import sys

byte_index = 0
with open(sys.argv[1], 'r') as file:
    while True:
        # example: 002E: E5
        line = file.readline()
        if len(line) == 0:
            break

        words = line.split()
        address = int(words[0].rstrip(':'), base=16)
        byte = int(words[1], base=16)

        # Fill with zeros
        while byte_index < address:
            sys.stdout.buffer.write((0x00).to_bytes(1, byteorder='little'))
            byte_index += 1

        sys.stdout.buffer.write(byte.to_bytes(1, byteorder='little'))
        byte_index += 1

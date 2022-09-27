#!/usr/bin/env python3
import sys

c_arr = ''
with open(sys.argv[1], 'rb') as f:
    while byte := f.read(1):
        c_arr += '0x%s, ' % byte.hex()

print('{ ' + c_arr.rstrip(', ') + ' }')

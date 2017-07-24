#!/usr/bin/env python3

import sys
import xmltodict

def split_map(m):
    for n in m.split('\n'):
        first = True
        for c in n.split(','):
            if c:
                if not first:
                    print(' ', end='')
                else:
                    first = False
                print('{:03}'.format(int(c)), end='')
        print()

with open(sys.argv[1]) as f:
    map = xmltodict.parse(f.read())

print(map['map']['layer'][0]['@name'])
split_map(map['map']['layer'][0]['data']['#text'])
print()

print(map['map']['layer'][1]['@name'])
split_map(map['map']['layer'][1]['data']['#text'])

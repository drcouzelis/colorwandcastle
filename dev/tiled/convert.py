#!/usr/bin/env python3

# Arg 1 is the Tiled "TMX" filename
# Arg 2 is an optional existing level "DAT" filename

import sys
import xmltodict

def print_csv_map_from_tmx(csv_map):
    print('# 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15')
    print('#################################################################')
    i = 0
    for n in csv_map.split('\n'):
        first = True
        for c in n.split(','):
            if c:
                if not first:
                    print(' ', end='')
                else:
                    first = False
                print('{:03}'.format(int(c)), end='')
        print(' # {:2}'.format(i))
        i = i + 1

# Check for args
if len(sys.argv) < 2:
    print('Usage: ' + sys.argv[0] + ' filename.tmx <filename.dat>', file=sys.stderr)
    exit(1)

tmx_filename = sys.argv[1]

if len(sys.argv) > 2:
    dat_filename = sys.argv[2]
else:
    dat_filename = None

# Grab the contents of the TMX file...
with open(tmx_filename) as tmx_file:
    tmx_xml = xmltodict.parse(tmx_file.read())

tmx_data = dict()
for entry in tmx_xml['map']['layer']:
    tmx_data[entry['@name']] = entry['data']['#text']

# Grab the contents of the DAT file, if applicable...
if dat_filename:
    with open(dat_filename) as dat_file:
        skip_until_blank = False
        for line in dat_file:
            line = line.rstrip()
            if skip_until_blank and line == '':
                skip_until_blank = False
            elif skip_until_blank:
                pass
            elif line in tmx_data.keys():
                skip_until_blank = True
                print(line)
                print_csv_map_from_tmx(tmx_data[line])
                print()
            else:
                print(line)

else:
    # Just convert and print the TMX file
    for entry in tmx_data.keys():
        print(entry)
        print_csv_map_from_tmx(tmx_data[entry])
        print()

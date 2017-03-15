#!/usr/bin/env python3
import xmltodict
with open('room-level-001.tmx') as f:
    map = xmltodict.parse(f.read())
print(map['map']['layer'][0]['@name'])
print(map['map']['layer'][0]['data']['#text'])

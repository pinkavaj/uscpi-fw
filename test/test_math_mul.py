#!/usr/bin/env python3

import math
import random
import serial
import sys


count=100

random.seed()
port=serial.Serial(port=sys.argv[1], baudrate=sys.argv[2], timeout=1)
port.open()

def test(mul1, mul2):
    cmd = 'test:mul %i,%i\n' % (mul1, mul2 )
    exp = mul1*mul2
    expHi = int(exp / (2**32))
    expLo = int(exp % (2**32))
    try:
        port.write(cmd.encode('ascii'))
        port.write('test:mul?\n'.encode('ascii'))
        res = port.readline()
        res = res.strip()
        resHi, resLo = res.decode('ascii').split(',')
        resHi = int(resHi)
        resLo = int(resLo)
        if resHi != expHi or resLo != expLo:
            raise Exception()
        print('.',end='')
        sys.stdout.flush()
    except:
        print ("Failed cmd: \n%s \nres: %s exp: %s\n" % (cmd, repr(res),
            repr(exp), ))
        raise

data = ((0,1),
        (1,1),
        (0xff,0x100),
        (0x10,0x7fffffff),
        (0x11,0x80000000),
        (0x12,0x9f000000),
        (0x12,0x9f000001),
        (0xffffffff,0xffffffff),
        )

for mulHi, mulLo in data:
    test(mulHi, mulLo)


i=1
mul_max_log = math.log(2**32-1, 10)
while i <= count:
    mulHi = int(10**(random.random()*mul_max_log)) - 1
    mulLo = int(10**(random.random()*mul_max_log)) - 1
    test(mulHi, mulLo)
    i = i + 1


print("OK")


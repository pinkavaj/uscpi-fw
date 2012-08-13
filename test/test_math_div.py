#!/usr/bin/env python3

import math
import random
import serial
import sys
import time


count=100

random.seed()
port=serial.Serial(port=sys.argv[1], baudrate=sys.argv[2], timeout=1)
port.open()

def test(nomHi, nomLo, num):
    cmd = 'test:div %i,%i,%i\n' % (nomHi, nomLo, num, )
    try:
        port.write(cmd.encode('ascii'))
        # FIXME: proč je potřeba tahle pauza?
        time.sleep(0.005)
        port.write('test:div?\n'.encode('ascii'))
        res = port.readline()
        res = int(res)
        exp = int(((2**32*nomHi) + nomLo)/num)
        if res != exp:
            raise Exception()
        print('.',end='')
        sys.stdout.flush()
    except:
        print ("Failed cmd: \n%s \nres: %s exp: %s\n" % (cmd, repr(res),
            repr(exp), ))
        raise

data = ((0,1,1),
        (1,1,2),
        (0xff,1,0x100),
        (0x10,1,0x7fffffff),
        (0x11,1,0x80000000),
        (0x12,1,0x9f000000),
        (0x12,1,0x9f000001),
        (0x13,1,0xffffffff),
        )

for nomHi, nomLo, num in data:
    test(nomHi, nomLo, num)


i=1
nom_max_log = math.log(2**64, 10)
num_max_log = math.log(2**32-1, 10)
while i <= count:
    nom = int(10**(random.random()*nom_max_log)) - 1
    nomHi = int(nom/(2**32))
    nomLo = nom % (2**32)
    num_min_log = math.log(nomHi+1, 10)
    num = int(10**(num_min_log + random.random()*(num_max_log-num_min_log)))
    test(nomHi, nomLo, num)
    i = i + 1


print("OK")


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

def test(num):
    """Test input for specified number"""
    exp = num
    res = None
    cmd = 'test:num %i\n' % num
    try:
        port.write(cmd.encode('ascii'))
        # FIXME: proč je potřeba tahle pauza?
        time.sleep(0.005)
        port.write('test:num?\n'.encode('ascii'))
        res = port.readline()
        res = int(res)
        if res != exp:
            raise Exception()
        print('.',end='')
        sys.stdout.flush()
    except:
        print ("Failed cmd: \n%s \nres: %s exp: %s\n" % 
                (cmd, repr(res), repr(exp), ))
        raise

for n in (0, 1, 2, 3, 4, 0xff, 0x100, 0xfffffff1, 0xfffffffe, 0xffffffff):
    test(n)

i=1
num_max_log=math.log((2**31), 10)
while i <= count:
    num = int(10**(random.random()*num_max_log)*2) - 1
    test(num)
    i = i + 1

print("OK")


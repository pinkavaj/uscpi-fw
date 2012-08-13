#!/usr/bin/env python3

import random
import serial
import sys
import time


count=1000

random.seed()
port=serial.Serial(port=sys.argv[1], baudrate=sys.argv[2], timeout=1)
port.open()
print(sys.argv[1])

i=0
while i < count:
    nomA = random.randint(0,2**16-2)
    nomB = random.randint(0,2**16-1)
    num = random.randint(nomA+1,2**32-1)
    cmd = 'test:div %i,%i,%i\n' % (nomA, nomB, num, )
    try:
        port.write(cmd.encode('ascii'))
        time.sleep(0.5)
        port.write('test:div?\n'.encode('ascii'))
        res = port.readline()
        res = int(res)
        exp = int(((nomA*2**32) + nomB)/num)
        if res != exp:
            raise Exception("Failed cmd: %s res: %s exp: %s" % (cmd, repr(res),
                repr(exp)))
        i = i + 1
    except:
        print ("Failed cmd: \n%s \nres: %s exp: %s\n" % (cmd, repr(res),
            repr(exp), ))
        raise


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

i=1
nom_max_log = math.log(2**64, 10)
num_max_log = math.log(2**32-1, 10)
while i <= count:
    nom = int(10**(random.random()*nom_max_log)) - 1
    nomHi = int(nom/(2**32))
    nomLo = nom % (2**32)
    num_min_log = math.log(nomHi+1, 10)
    num = int(10**(num_min_log + random.random()*(num_max_log-num_min_log)))

    cmd = 'test:div %i,%i,%i\n' % (nomHi, nomLo, num, )
    try:
        port.write(cmd.encode('ascii'))
        # FIXME: proč je potřeba tahle pauza?
        time.sleep(0.005)
        port.write('test:div?\n'.encode('ascii'))
        res = port.readline()
        res = int(res)
        exp = int(((nomHi*2**32) + nomLo)/num)
        if res != exp:
            raise Exception()
        print('.',end='')
        sys.stdout.flush()
        i = i + 1
    except:
        print ("Failed cmd: \n%s \nres: %s exp: %s\n" % (cmd, repr(res),
            repr(exp), ))
        raise

print("OK")


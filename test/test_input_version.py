#!/usr/bin/env python3

import serial
import sys


count=100

port=serial.Serial(port=sys.argv[1], baudrate=sys.argv[2], timeout=1)
port.open()

def test(mult):
    """Test input for specified number"""
    cmd = 'syst:vers?\n' * mult
    exp = '1999.0'
    try:
        port.write(cmd.encode('ascii'))
        while mult:
            res = port.readline()
            res = res.decode('ascii').strip()
            if res != exp:
                raise Exception()
            print('.',end='')
            sys.stdout.flush()
            mult = mult - 1
    except:
        print ("Failed cmd: \n%s \nres: %s exp: %s\n" % 
                (cmd, repr(res), repr(exp), ))
        raise

# cmd burst
for n in (1, 2, 3, 4, 16, 30, 60 ):
    test(n)

print("OK")


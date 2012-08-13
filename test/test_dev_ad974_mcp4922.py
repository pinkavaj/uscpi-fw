#!/usr/bin/env python2

import serial
import sys
import time


count=100

port=serial.Serial(port=sys.argv[1], baudrate=sys.argv[2], timeout=1)
port.open()

def test(count):
    """Test input for specified number"""
    cmd = 'test:adc?\n'
    d_max = 300
    try:
        port.write(cmd.encode('ascii'))
        res = port.readline()
        res = res.decode('ascii').strip()
        ch0a, ch0b, ch1a, ch1b, ch2a, ch2b, ch3a, ch3b = res.split(',')
        ch0a = int(ch0a)
        ch1a = int(ch1a)
        ch2a = int(ch2a)
        ch3a = int(ch3a)
        ch0b = int(ch0b)
        ch1b = int(ch1b)
        ch2b = int(ch2b)
        ch3b = int(ch3b)
	d0 = ch0a - ch0b
	d1 = ch1a - ch1b
	d2 = ch2a - ch2b
	d3 = ch3a - ch3b
        if abs(d0) > d_max or \
                        abs(d1) > d_max or \
                        abs(d2) > d_max or \
                        abs(d3) > d_max:
            raise Exception()
        if count % 200 == 0:
		sys.stdout.write('.')
		sys.stdout.flush()
    except:
        print ("Failed cmd: \n%s \nres: %s dif: %i %i %i %i\n" % 
		(cmd, repr(res), d0, d1, d2, d3, ))
	print ("count: %i" % count)
        raise

port.write('test:adc 400,400\n'.encode('ascii'))
time.sleep(0.01)

# cmd burst
count = 0
while True:
    test(count)
    count = count + 1

print("OK")


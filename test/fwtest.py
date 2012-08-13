#!python3

import serial
import sys
import time
import unittest

class FwTest(unittest.TestCase):
    """Common IO part for SCPI device testing. Read serial port name and
    baudrate from program parameters."""
    def setUp(self):
        self.port = serial.Serial('/dev/ttyUSB0', baudrate=115200, 
                timeout=1)
        self.port.write('\x03'.encode('ascii'))
        time.sleep(0.1)

    def writeline(self, line):
        """Write line into device, append newline."""
        line = line + '\n'
        self.port.write(line.encode('ascii'))

    def readline(self):
        """Read line from device, return striped line or None."""
        line = self.port.readline()
        if not line:
            return None
        return line.strip().decode('ascii')


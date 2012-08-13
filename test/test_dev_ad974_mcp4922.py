#!/usr/bin/env python3

from fwtest import FwTest

import unittest


class DevAD974Test(FwTest):
    def setUp(self):
        FwTest.setUp(self)
        #port.write('test:adc 400,400\n'.encode('ascii'))

    def test_ad974(self):
        cmd = 'test:adc?'
        d_max = 400
        
        count=100
        while count:
            count = count - 1
            self.writeline(cmd)
            res = self.readline()
            ch0a, ch0b, ch1a, ch1b, ch2a, ch2b, ch3a, ch3b = res.split(',')
            ch0a, ch1a, ch2a, ch3a, ch0b, ch1b, ch2b, ch3b = \
                    int(ch0a), int(ch1a), int(ch2a), int(ch3a), \
                    int(ch0b), int(ch1b), int(ch2b), int(ch3b)
# TODO: udělat test chXY != 0 -> aby se ověřilo že je AD připojeno
            d0 = abs(ch0a - ch0b)
            d1 = abs(ch1a - ch1b)
            d2 = abs(ch2a - ch2b)
            d3 = abs(ch3a - ch3b)
            self.assertLess(d0, d_max)
            self.assertLess(d1, d_max)
            self.assertLess(d2, d_max)
            self.assertLess(d3, d_max)


if __name__ == '__main__':
    unittest.main()


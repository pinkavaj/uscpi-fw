#!/usr/bin/env python3

from fwtest import FwTest

import unittest


class InputTest(FwTest):
    def setUp(self):
        FwTest.setUp(self)

    def test_input_version(self):
        # cmd burst
        for mult in (1, 2, 3, 4, 16, 30, 60 ):
            cmd = 'syst:vers?\n' * mult
            exp = '1999.0'
            self.writeline(cmd)
            while mult:
                res = self.readline()
                self.assertEqual(res, exp)
                mult = mult - 1


if __name__ == '__main__':
    unittest.main()


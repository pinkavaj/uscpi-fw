#!/usr/bin/env python3

from fwtest import FwTest

import unittest


class StatusReportingTest(FwTest):
    def setUp(self):
        FwTest.setUp(self)

    def test_StatusRegisterEnabled(self):
        exp = 0
        self.writeline('*SRE 0')
        self.writeline('*SRE?')
        res = self.readline()
        res = int(res)
        self.assertEqual(res, exp)
        exp = 0b10111111
        self.writeline('*SRE 255')
        self.writeline('*SRE?')
        res = self.readline()
        res = int(res)
        self.assertEqual(res, exp)

    def test_StandardEventStatusEnabled(self):
        for exp in (0, 0xff, ):
            self.writeline('*ESE %i' % exp)
            self.writeline('*ESE?')
            res = self.readline()
            res = int(res)
            self.assertEqual(res, exp)

    def test_StatusByte(self):
        exp = 0
        self.writeline('*STB?')
        res = self.readline()
        res = int(res)
        self.assertEqual(res, exp)
        # test single bits
        # invalid command
        self.writeline('*ESE 255')
        self.writeline('*SRE 0')
        self.writeline('bagr')
        exp = 0
        self.writeline('*STB?')
        res = self.readline()
        res = int(res)
        self.assertEqual(res, exp)

        exp = 0b01100100
        self.writeline('*SRE 255')
        self.writeline('*STB?')
        res = self.readline()
        res = int(res)
        self.assertEqual(res, exp)


if __name__ == '__main__':
    unittest.main()


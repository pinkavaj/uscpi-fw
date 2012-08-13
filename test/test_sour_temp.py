#!/usr/bin/env python3

from fwtest import FwTest
from pt import Pt

import random
import unittest


class TempTest(FwTest):
    Tmin = 30
    Tmax = 30 + 256 + 512 - 1

    def setUp(self):
        FwTest.setUp(self)
        count = 100
        self.Pt = Pt(1.)

        Rmin = self.Pt.resistance(self.Tmin - 1)
        self.Rmin = self.Pt.resistance(self.Tmin)
        RmaxA = self.Pt.resistance(self.Tmax - 1)
        self.Rmax = self.Pt.resistance(self.Tmax)
        RmaxZ = self.Pt.resistance(self.Tmax + 1)
        self.R = [0., 0.98, 1.0, Rmin, self.Rmin, RmaxA, self.Rmax, RmaxZ, ]
        self.T = [0., self.Tmin - 0.05, self.Tmin, self.Tmax - 0.05, self.Tmax,
                self.Tmax + 0.05, ]
        random.seed()
        while count:
            count = count - 1
            self.R.append(
                    random.random() * (self.Rmax - self.Rmin) + self.Rmin)
            self.T.append(random.random() * (self.Tmax - self.Tmin) + self.Tmin)

    def test_temp(self):
        """Test input for specified numbers"""

        for num in self.R:
            exp = 0
            if num >= self.Rmax:
                exp = 0xffff / 20.
            elif num >= self.Rmin:
                exp = self.Pt.temperature(num)
            cmd = 'test:temp? %i' % round(num*(2**14))
            self.writeline(cmd)
            res = self.readline()
            res = int(res) / 20.
            try:
                self.assertEqual(round(res*20), round(exp*20), 
                        cmd + " (res: %f exp: %f" % (res, exp))
            except AssertionError:
                x = abs(res - exp)
                self.assertLess(x, 0.05, 
                        cmd + " (res: %f exp: %f" % (res, exp))

    def test_temp_res(self):
        """Test input for specified numbers"""

        fix_point = (2**14)
        for num in self.T:
            exp = 0
            if num >= self.Tmax:
                exp = 0xffff / fix_point
            elif num >= self.Tmin:
                exp = self.Pt.resistance(num)
            cmd = 'test:temp:res? %i' % round(num*20)
            self.writeline(cmd)
            res = self.readline()
            res = int(res) / fix_point
            try:
                self.assertEqual(round(res), round(exp), 
                        cmd + " (res: %f exp: %f" % (res, exp))
            except AssertionError:
                x = abs(res - exp)
                self.assertLess(x, 0.05, 
                        cmd + " (res: %f exp: %f" % (res, exp))


if __name__ == '__main__':
    unittest.main()


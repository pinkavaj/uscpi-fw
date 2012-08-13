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

        Rmin = self.Pt.resistivity(self.Tmin - 1)
        self.Rmin = self.Pt.resistivity(self.Tmin)
        RmaxA = self.Pt.resistivity(self.Tmax - 1)
        self.Rmax = self.Pt.resistivity(self.Tmax)
        RmaxZ = self.Pt.resistivity(self.Tmax + 1)
        self.R = [0., 0.98, 1.0, Rmin, self.Rmin, RmaxA, self.Rmax, RmaxZ, ]
        random.seed()
        while count:
            count = count - 1
            self.R.append(
                    random.random() * (self.Rmax - self.Rmin) + self.Rmin)

    def test_num(self):
        """Test input for specified numbers"""

        for num in self.R:
            exp = 0
            if num >= self.Rmax:
                exp = 0xffff / 20.
            elif num >= self.Rmin:
                exp = self.Pt.temperature(num)
            cmd = 'test:temp? %i' % round(num*(2**14))
            print(cmd)
            self.writeline(cmd)
            res = self.readline()
            res = int(res) / 20.
            try:
                self.assertEqual(round(res*20), round(exp*20))
            except AssertionError:
                x = abs(res - exp)
                self.assertLess(x, 0.05)


if __name__ == '__main__':
    unittest.main()


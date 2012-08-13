#!/usr/bin/env python3

from fwtest import FwTest

import random
import unittest


class FixedPointTest(FwTest):
    "Test fixed-point conversion from text to number and back"
    def setUp(self):
        FwTest.setUp(self)
        count = 200

        self.numbers = [0, 0.00001, 0.00002, 0.00003, 0.99999, 0.99998, 
                0.99997, 0.99996, ]
        random.seed()
        while count:
            count = count - 1
            self.numbers.append(random.random())

    def test_fix_point(self):
        for num in self.numbers:
            exp = "%.5f" % num
            cmd = 'test:func:dec? %.5f' % num
            self.writeline(cmd)
            res = self.readline()
            try:
                self.assertEqual(res, exp)
            except:
                cmd_ = 'test:func:dec? %s' % res
                self.writeline(cmd)
                res_ = self.readline()
                self.assertEqual(res_, res)
                res = res_
            res = float(res)
            exp = float(exp)
            x = abs(res - exp)
            self.assertLessEqual(x, 0.0000101, "res: %f, exp: %f, x:%f " % 
                    (res, exp, x,) )


if __name__ == '__main__':
    unittest.main()


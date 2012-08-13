#!/usr/bin/env python3

from fwtest import FwTest

import math
import random
import unittest


class NumTest(FwTest):
    def setUp(self):
        FwTest.setUp(self)
        count = 100
        random.seed()

        self.data = [(0,1),
                (1,1),
                (0xff,0x100),
                (0x10,0x7fffffff),
                (0x11,0x80000000),
                (0x12,0x9f000000),
                (0x12,0x9f000001),
                (0xffffffff,0xffffffff),
                ]
        mul_max_log = math.log(2**32-1, 10)
        while count:
            count = count - 1
            mulHi = int(10**(random.random()*mul_max_log)) - 1
            mulLo = int(10**(random.random()*mul_max_log)) - 1
            self.data.append((mulHi, mulLo, ))

    def test_math_mull(self):

        for mul1, mul2 in self.data:
            cmd = 'test:mul %i,%i' % (mul1, mul2 )
            exp = mul1*mul2
            exp = (int(exp / (2**32)), int(exp % (2**32)))
            self.writeline(cmd)
            self.writeline('test:mul?')
            res = self.readline()
            res = res.split(',')
            res = (int(res[0]), int(res[1]))
            self.assertEqual(res, exp, "cmd: \n%s" % (cmd, ))


if __name__ == '__main__':
    unittest.main()


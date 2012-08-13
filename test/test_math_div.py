#!/usr/bin/env python3

from fwtest import FwTest

import math
import random
import unittest


class NumTest(FwTest):
    def setUp(self):
        FwTest.setUp(self)
        random.seed()
        count = 100

        self.data = [(0,1,1),
                (1,1,2),
                (0xff,1,0x100),
                (0x10,1,0x7fffffff),
                (0x11,1,0x80000000),
                (0x12,1,0x9f000000),
                (0x12,1,0x9f000001),
                (0x13,1,0xffffffff),
                ]

        nom_max_log = math.log(2**64, 10)
        num_max_log = math.log(2**32-1, 10)
        while count:
            count = count - 1
            nom = int(10**(random.random()*nom_max_log)) - 1
            nomHi = int(nom/(2**32))
            nomLo = nom % (2**32)
            num_min_log = math.log(nomHi+1, 10)
            num = int(10**(num_min_log + random.random()*(num_max_log-num_min_log)))
            self.data.append ((nomHi, nomLo, num, ))

    def test_math_div(self):
        for nomHi, nomLo, num in self.data:
            cmd = 'test:div %i,%i,%i' % (nomHi, nomLo, num, )
            exp = int(((2**32*nomHi) + nomLo)/num)
            self.writeline(cmd)
            self.writeline('test:div?')
            res = self.readline()
            res = int(res)
            self.assertEqual(res, exp, "cmd: \n%s" % (cmd, ))


if __name__ == '__main__':
    unittest.main()


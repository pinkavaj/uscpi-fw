#!/usr/bin/env python3

from fwtest import FwTest

import math
import random
import unittest


class NumTest(FwTest):
    def setUp(self):
        FwTest.setUp(self)
        count = 100

        self.numbers = [0, 1, 2, 3, 4, 0xff, 0x100, 0xfffffff1, 0xfffffffe, 
                0xffffffff]
        random.seed()
        num_max_log=math.log((2**31), 10)
        while count:
            count = count - 1
            self.numbers.append(int(10**(random.random()*num_max_log)*2) - 1)

    def test_num(self):
        """Test input for specified numbers"""

        for num in self.numbers:
            exp = num
            cmd = 'test:num %i' % num
            self.writeline(cmd)
            self.writeline('test:num?')
            res = self.readline()
            res = int(res)
            self.assertEqual(res, exp)


if __name__ == '__main__':
    unittest.main()


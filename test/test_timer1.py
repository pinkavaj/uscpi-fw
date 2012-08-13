#!/usr/bin/env python3

from fwtest import FwTest

import time
import unittest


class TimeSecTest(FwTest):
    def setUp(self):
        FwTest.setUp(self)
        count = 100

        self.delays = [0, 2, 2, 3, 4]

    def test_num(self):
        """Test input for specified numbers"""
        exp = 0
        for d in self.delays:
            exp = exp + d
            time.sleep(d)
            self.writeline('test:time?')
            res = self.readline()
            res = int(res)
            self.assertEqual(res, exp)


if __name__ == '__main__':
    unittest.main()


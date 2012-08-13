#!python
# -*- coding: utf-8 -*-

from math import sqrt

class Pt:
    "Compute resistance and temperature fot Pt conductor"

    A = 3.908e-3
    B = -5.802e-7

    def __init__(self, R0):
        self.R0 = float(R0)

    def temperature(self, R):
        "Compute temperature from resistance."
        a = 2 * self.R0 * self.B
        b = self.R0 * self.A
        c = self.R0 - R
        return ((-b) + sqrt( b ** 2 - 2 * a * c)) / a

    def resistance(self, temp):
        "Compute resistance from temperature (°C)."
        temp = float(temp)
        return self.R0 * (1 + (self.A * temp) + self.B * (temp ** 2))

###### SELF-TEST ######
if __name__ == '__main__':
    from pt import Pt

    s = Pt(10)
    print("R0 = 10{\Omega}")
    print("T = Pt(R=13) = %f" % s.temperature(13))
    print("R = Pt(T=105°C) = %f" % s.resistance(105))


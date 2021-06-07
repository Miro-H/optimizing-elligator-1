#!/usr/bin/env sage

class Curve1174:  # struct in c  # edward curve
    def __init__(self):
        self.q = 2^251 - 9
        self.Fq = GF(self.q)

        # Use Montogomery curve, which is equivalent to the following Edwards curve:
        # x^2 + y^2 = 1 - 1174 * x^2 * y^2
        self.curve = EllipticCurve(self.Fq, [0, 4/1175-2, 0, 1, 0])

        self.s = self.Fq(1806494121122717992522804053500797229648438766985538871240722010849934886421)
        self.c = 2 / (self.s^2)
        self.d = -(self.c + 1)^2 / (self.c - 1)^2
        self.r = self.c + 1 / self.c

        # Sanity checks
        assert(self.q % 4 == 3)
        assert(self.c * (self.c - 1) * (self.c + 1) != 0)
        assert(self.d == -1174)
        assert(self.r != 0)
        assert(not self.d.is_square())

curve = Curve1174()

def chi(x, q):
    return legendre_symbol(x, q)

def elligator1_map(t):
    if t in [1, -1]:
        return 0, 1

    u = (1 - t) / (1 + t)
    v = u^5 + (curve.r^2 - 2) * u^3 + u

    chi_v = chi(v, curve.q)
    X = chi_v * u
    Y = (chi_v * v)^((curve.q + 1)/4) * chi_v * chi(u^2 + 1/curve.c^2, curve.q)

    x = (curve.c - 1) * curve.s * X * (1 + X) / Y
    y = (curve.r * X - (1 + X)^2) / (curve.r * X + (1 + X)^2)

    # Sanity checks
    assert(x^2 + y^2 == 1 + curve.d * x^2 * y^2)
    assert(u * v * X * Y * x * (y + 1) != 0)
    assert(Y^2 == X^5 + (curve.r^2 - 2) * X^3 + X)

    return x, y

def elligator1_invmap(x, y):
    assert(y + 1 != 0)

    eta = (y - 1) / (2 * (y + 1))

    # Two sanity checks
    assert(((1 + eta * curve.r)^2 - 1).is_square())
    if eta * curve.r == -2:
        assert(x == 2 * curve.s * (curve.c - 1) * chi(curve.c, curve.q) / curve.r)

    X = -(1 + eta * curve.r) + ((1 + eta * curve.r)^2 - 1)^((curve.q + 1) / 4)
    z = chi((curve.c - 1) * curve.s * X * (1 + X) * x * (X^2 + 1 / curve.c^2), curve.q)
    u = z * X
    t = (1 - u) / (1 + u)

    if t.lift() > (curve.q - 1) / 2:
        return -t
    return t

if __name__ == '__main__':
    t_orig = 7
    x, y = elligator1_map(curve.Fq(t_orig))
    print(f"x = {x}\ny = {y}")

    t = elligator1_invmap(x, y)
    print(f"t = {t}")

    assert(t == t_orig)
#!/usr/bin/env python3

################################################################################
#
# This file is part of the ASL project "Censorship-avoiding high-speed EC
# (Elligator with Curve1174)"
# (https://github.com/Miro-H/optimizing-elligator-1).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Contact: miro.haller@alumni.ethz.ch
#
# Short description of this file:
# This is a Python implementation for Elligator 1 that mimics BigInt operations
# used to verify our C implementation.
#
################################################################################


class BigInt:  # struct in c
    def __init__(self, x):
        self.x = x


class CurvePoint:  # struct in c
    def __init__(self, x, y):
        self.x = x
        self.y = y


class Curve1174:  # struct in c  # edward curve
    def __init__(self):
        self.q = create_big_int(2 ** 251 - 9)
        self.d = big_int_mod(create_big_int(-1174), self.q)
        self.s = create_big_int(1806494121122717992522804053500797229648438766985538871240722010849934886421)

        c0 = big_int_mul(self.s, self.s)
        c0 = big_int_inverse(c0, self.q)
        c0 = big_int_mul(create_big_int(2), c0)
        c0 = big_int_mod(c0, self.q)
        self.c = c0  # c = 2/(s**2)

        r0 = big_int_inverse(self.c, self.q)
        r0 = big_int_add(self.c, r0)
        r0 = big_int_mod(r0, self.q)
        self.r = r0  # r = c + 1/c


def create_big_int(x):
    return BigInt(x)


def big_int_pow(a, b, q):
    return BigInt(fast_pow(a, b, q))


def big_int_mul(a, b):
    return BigInt(a.x * b.x)


def big_int_div(a, b):
    return BigInt(a.x // b.x)


def big_int_add(a, b):
    return BigInt(a.x + b.x)


def big_int_sub(a, b):
    return BigInt(a.x - b.x)


def big_int_negate(a):
    return BigInt(-a.x)


def big_int_mod(a, p):
    return BigInt(a.x % p.x)


def big_int_compare(a, b):
    if a.x == b.x:
        return 0
    elif a.x < b.x:
        return -1
    else:
        return 1


def egcd(a, b):
    # Euclidean algorithm for gcd computation
    if big_int_compare(a, create_big_int(0)) == 0:
        return b, create_big_int(0), create_big_int(1)
    else:
        g, y, x = egcd(big_int_mod(b, a), a)
        r0 = big_int_div(b, a)
        r0 = big_int_mul(r0, y)
        return g, big_int_sub(x, r0), y


def big_int_inverse(a, p):
    # Modular inversion computation
    if big_int_compare(a, create_big_int(0)) < 0:
        r0 = big_int_negate(a)
        r0 = big_int_inverse(r0, p)
        return big_int_sub(p, r0)
    g, x, y = egcd(a, p)
    if big_int_compare(g, create_big_int(1)) != 0:
        raise ArithmeticError("Modular inverse does not exist")
    else:
        return big_int_mod(x, p)


def fast_pow(b, p, m):
    b = b.x
    p = p.x
    m = m.x
    result = 1
    while p > 0:
        # If power is odd
        if p % 2 == 1:
            result = (result * b) % m

        # Divide the power by 2
        p = p // 2
        # Multiply base to itself
        b = (b * b) % m

    return result


def chi(t, curve):  # chi(a) = a**((q-1)/2)
    r1 = big_int_sub(curve.q, create_big_int(1))
    r0 = big_int_div(r1, create_big_int(2))
    return big_int_pow(t, r0, curve.q)


def elligator_1_string_to_point(t, curve):
    if t in [-1, 1]:
        return CurvePoint(0, 1)

    u0 = big_int_sub(create_big_int(1), t)
    u1 = big_int_add(create_big_int(1), t)
    u1 = big_int_inverse(u1, curve.q)
    u0 = big_int_mul(u0, u1)
    u = big_int_mod(u0, curve.q)  # u = (1 − t) / (1 + t),

    v0 = big_int_pow(u, create_big_int(5), curve.q)
    v1 = big_int_pow(curve.r, create_big_int(2), curve.q)
    v1 = big_int_sub(v1, create_big_int(2))
    v2 = big_int_pow(u, create_big_int(3), curve.q)
    v1 = big_int_mul(v1, v2)
    v0 = big_int_add(v0, v1)
    v0 = big_int_add(v0, u)
    v = big_int_mod(v0, curve.q)  # v = u**5 + (r**2 − 2)*u**3 + u,

    CHIV = chi(v, curve)

    X0 = big_int_mul(CHIV, u)
    X = big_int_mod(X0, curve.q)  # X = χ(v)u

    Y0 = big_int_mul(CHIV, v)
    Y1 = big_int_add(curve.q, create_big_int(1))
    Y2 = big_int_div(Y1, create_big_int(4))
    Y0 = big_int_pow(Y0, Y2, curve.q)
    Y0 = big_int_mul(Y0, CHIV)

    C2 = big_int_mul(curve.c, curve.c)
    C2 = big_int_inverse(C2, curve.q)
    Y3 = big_int_mul(u, u)
    Y3 = big_int_add(Y3, C2)
    Y3 = chi(Y3, curve)

    Y0 = big_int_mul(Y0, Y3)
    Y = big_int_mod(Y0, curve.q)  # Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2),

    X_plus_1 = big_int_add(create_big_int(1), X)
    X_plus_1_squared = big_int_mul(X_plus_1, X_plus_1)

    x0 = big_int_sub(curve.c, create_big_int(1))
    x0 = big_int_mul(x0, curve.s)
    x0 = big_int_mul(x0, X)
    x0 = big_int_mul(x0, X_plus_1)
    x1 = big_int_inverse(Y, curve.q)
    x0 = big_int_mul(x0, x1)
    x = big_int_mod(x0, curve.q)  # x = (c − 1)*s*X*(1 + X) / Y

    rX = big_int_mul(curve.r, X)
    y0 = big_int_sub(rX, X_plus_1_squared)

    y1 = big_int_add(rX, X_plus_1_squared)
    y1 = big_int_inverse(y1, curve.q)

    y0 = big_int_mul(y0, y1)
    y = big_int_mod(y0, curve.q)  # y = (rX − (1 + X)**2) / (rX + (1 + X)**2)

    return CurvePoint(x, y)


def elligator_1_point_to_string(p, curve):
    eta0 = big_int_sub(p.y, create_big_int(1))
    eta1 = big_int_add(p.y, create_big_int(1))
    eta1 = big_int_mul(create_big_int(2), eta1)
    eta1 = big_int_inverse(eta1, curve.q)
    eta0 = big_int_mul(eta0, eta1)
    eta = big_int_mod(eta0, curve.q)  # η = (y-1)/(2(y+1))

    eta_r = big_int_mul(eta, curve.r)
    eta_r = big_int_mod(big_int_add(create_big_int(1), eta_r), curve.q)
    q_1 = big_int_add(curve.q, create_big_int(1))
    q_1 = big_int_div(q_1, create_big_int(4))

    X1 = big_int_mul(eta_r, eta_r)
    X1 = big_int_sub(X1, create_big_int(1))
    X1 = big_int_pow(X1, q_1, curve.q)
    X1 = big_int_sub(X1, eta_r)
    X = big_int_mod(X1, curve.q)  # X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)

    # The following is true only if ηr == -2
    # As of now, we don't store points in compressed form, so this doesn't matter.

    # x0 = big_int_mul(create_big_int(2), curve.s)
    # x1 = big_int_sub(curve.c, create_big_int(1))
    # x0 = big_int_mul(x0, x1)
    # x2 = chi(curve.c, curve)
    # x0 = big_int_mul(x0, x2)
    # x3 = big_int_inverse(curve.r, curve.q)
    # x0 = big_int_mul(x0, x3)
    # x = big_int_mod(x0, curve.q)  # x = 2s(c − 1)χ(c)/r

    x = p.x

    z0 = big_int_sub(curve.c, create_big_int(1))
    z0 = big_int_mul(z0, curve.s)
    z0 = big_int_mul(z0, X)
    z1 = big_int_add(create_big_int(1), X)
    z0 = big_int_mul(z0, z1)
    z0 = big_int_mul(z0, x)
    z2 = big_int_mul(X, X)
    z3 = big_int_mul(curve.c, curve.c)
    z3 = big_int_inverse(z3, curve.q)
    z2 = big_int_add(z2, z3)
    z0 = big_int_mul(z0, z2)
    z0 = chi(z0, curve)
    z = big_int_mod(z0, curve.q)  # z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))

    u0 = big_int_mul(z, X)
    u = big_int_mod(u0, curve.q)  # u = zX

    t0 = big_int_sub(create_big_int(1), u)
    t1 = big_int_add(create_big_int(1), u)
    t1 = big_int_inverse(t1, curve.q)
    t0 = big_int_mul(t0, t1)
    t = big_int_mod(t0, curve.q)  # t = (1 − u)/(1 + u)

    q_half = big_int_sub(curve.q, create_big_int(1))
    q_half = big_int_div(curve.q, create_big_int(2))
    if big_int_compare(t, q_half) == 1:
        t_neg = big_int_negate(t)
        return big_int_mod(t, curve.q)

    return t


if __name__ == "__main__":
    curve = Curve1174()
    s1 = create_big_int(7)
    s2 = create_big_int(3)

    assert(s1.x == 7 and s2.x == 3)
    assert(big_int_negate(s1).x == -7)
    assert(big_int_add(s1, s2).x == 10)
    assert(big_int_sub(s1, s2).x == 4)
    assert(big_int_inverse(s2, s1).x == 5)
    assert(big_int_mod(s2, s1).x == 3)
    assert(big_int_mul(s1, s2).x == 21)
    assert(big_int_pow(s1, s2, curve.q).x == 343)
    assert(big_int_div(s1, s2).x == 2)

    p = elligator_1_string_to_point(s1, curve)
    print(f"x = {p.x.x}\ny = {p.y.x}")
    t = elligator_1_point_to_string(p, curve)
    print(f"t = {t.x}")
    #print(curve.q.x)
    #print(curve.s.x)

    assert(s1.x == t.x)
    c = create_big_int(0x4D1A3398ED42CEEB451D20824CA9CB49B69EF546BD7E6546AEF19AF1F9E49E1)
    c_2 = big_int_mul(c, c)
    q = create_big_int(0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7)
    r = big_int_inverse(c_2, q)
    print(hex(r.x).upper())
    print()

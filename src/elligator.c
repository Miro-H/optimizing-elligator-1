#include "elligator.h"

BigInt create_big_int(uint64_t x)
{
    BigInt r = {x};
    return r;
}

BigInt big_int_pow(BigInt base, BigInt exponent, BigInt q)
{
    BigInt result = create_big_int(1);
    BigInt p = exponent;
    BigInt b = base;

    while (big_int_compare(p, create_big_int(0)) > 0)
    {
        // If power is odd
        if (big_int_compare(big_int_mod(p, create_big_int(2)), create_big_int(1)) == 0)
        {
            result = big_int_mod(big_int_mul(result, b), q);
        }
        p = big_int_div(p, create_big_int(2));
        p = big_int_mod(big_int_mul(b, b), q);
    }
    return result;
}
/*
BigInt big_int_mul(BigInt a, BigInt b, BigInt q) // needs to be changed
{
    return BigInt((a.x * b.x) % q.x);
}
*/
BigInt big_int_mul(BigInt a, BigInt b) // needs to be changed
{
    BigInt r = {a.x * b.x};
    return r;
}
/*
BigInt big_int_div(BigInt a, BigInt b, BigInt q) // needs to be changed
{
    return BigInt((a.x / b.x) % q.x);
}
*/
BigInt big_int_div(BigInt a, BigInt b) // needs to be changed
{
    BigInt r = {a.x / b.x};
    return r;
}
/*
BigInt big_int_add(BigInt a, BigInt b, BigInt q) // needs to be changed
{
    return BigInt((a.x + b.x) % q.x);
}
*/
BigInt big_int_add(BigInt a, BigInt b) // needs to be changed
{
    BigInt r = {a.x + b.x};
    return r;
}
/*
BigInt big_int_sub(BigInt a, BigInt b, BigInt q) // needs to be changed
{
    return BigInt((a.x - b.x) % q.x);
}
*/
BigInt big_int_sub(BigInt a, BigInt b) // needs to be changed
{
    BigInt r = {a.x - b.x};
    return r;
}
/*
BigInt big_int_negate(BigInt a, BigInt q) // needs to be changed
{
    return BigInt((-a.x) % q.x);
}
*/
BigInt big_int_negate(BigInt a) // needs to be changed
{
    BigInt r = {-a.x};
    return r;
}

BigInt big_int_inverse(BigInt a, BigInt q) // a^(-1) mod q mod inverse
{
    // Modular inversion computation
    if (big_int_compare(a, create_big_int(0)) < 0)
    {
        BigInt r0 = big_int_negate(a);
        r0 = big_int_inverse(r0, q);
        return big_int_sub(q, r0);
    }

    egcd_result res = egcd(a, q);
    if (big_int_compare(q, create_big_int(1)) != 0)
    {
        return create_big_int(-1);
    }
    else
    {
        return big_int_mod(res.x, q);
    }
}

BigInt big_int_mod(BigInt a, BigInt q) // needs to be changed
{
    BigInt r = {a.x % q.x};
    return r;
}

uint64_t big_int_compare(BigInt a, BigInt b) // a==b: 0, a<b: -1, a>b: 1
{
    if (a.x == b.x)
    {
        return 0;
    }
    else if (a.x < b.x)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

egcd_result egcd(BigInt a, BigInt b) // Euclidean algorithm for gcd computation
{
    if (big_int_compare(a, create_big_int(0)) == 0)
    {
        egcd_result r = {b, create_big_int(0), create_big_int(1)};
        return r;
    }
    else
    {
        egcd_result res = egcd(big_int_mod(b, a), a);
        BigInt r0 = big_int_div(b, a);
        r0 = big_int_mul(r0, res.y);
        egcd_result r = {res.g, big_int_sub(res.x, r0), res.y};
        return r;
    }
}

BigInt chi(BigInt t, BigInt q) // chi function: chi(t) = t**((q-1)/2)
{
    BigInt r0 = big_int_inverse(create_big_int(2), q);
    BigInt r1 = big_int_sub(q, create_big_int(1));
    r0 = big_int_mul(r1, r0);
    r0 = big_int_pow(t, r0, q);
    return r0;
}
CurvePoint elligator_1_string_to_point(BigInt t, Curve curve)
{
    // TODO: assert t =/= +1, -1
    BigInt u0 = big_int_sub(create_big_int(1), t);
    BigInt u1 = big_int_add(create_big_int(1), t);
    u1 = big_int_inverse(u1, curve.q);
    u0 = big_int_mul(u0, u1);
    BigInt u = big_int_mod(u0, curve.q);  // u = (1 − t) / (1 + t)

    // TODO: Optimization potential; reuse partial results for powers, instead
    // of doing it from scratch each time
    BigInt v0 = big_int_pow(u, create_big_int(5), curve.q);
    BigInt v1 = big_int_pow(curve.r, create_big_int(2), curve.q);
    v1 = big_int_sub(v1, create_big_int(2));
    BigInt v2 = big_int_pow(u, create_big_int(3), curve.q);
    v1 = big_int_mul(v1, v2);
    v0 = big_int_add(v0, v1);
    v0 = big_int_add(v0, u);
    BigInt v = big_int_mod(v0, curve.q);  // v = u**5 + (r**2 − 2)*u**3 + u

    BigInt CHIV = chi(v, curve.q);

    BigInt X0 = big_int_mul(CHIV, u);
    BigInt X = big_int_mod(X0, curve.q); // X = χ(v)u

    BigInt Y0 = big_int_mul(CHIV, v);
    BigInt Y1 = big_int_add(curve.q, create_big_int(1));
    BigInt Y2 = big_int_inverse(create_big_int(4), curve.q);
    Y1 = big_int_mul(Y1, Y2);
    Y0 = big_int_pow(Y0, Y1, curve.q);
    Y0 = big_int_mul(Y0, CHIV);

    BigInt C2 = big_int_pow(curve.c, create_big_int(2), curve.q);
    C2 = big_int_inverse(C2, curve.q);
    BigInt Y3 = big_int_pow(u, create_big_int(2), curve.q);
    Y3 = big_int_add(Y3, C2);
    Y3 = big_int_mod(Y3, curve.q);
    Y3 = chi(Y3, curve.q);

    Y0 = big_int_mul(Y0, Y3);
    BigInt Y = big_int_mod(Y0, curve.q);  // Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2),

    BigInt X_plus_1 = big_int_add(create_big_int(1), X);
    BigInt X_plus_1_squared = big_int_pow(X_plus_1, create_big_int(2), curve.q);

    BigInt x0 = big_int_sub(curve.c, create_big_int(1));
    x0 = big_int_mul(x0, curve.s);
    x0 = big_int_mul(x0, X);
    x0 = big_int_mul(x0, X_plus_1);
    BigInt x1 = big_int_inverse(Y, curve.q);
    x0 = big_int_mul(x0, x1);
    BigInt x = big_int_mod(x0, curve.q);  // x = (c − 1)*s*X*(1 + X) / Y

    BigInt rX = big_int_mul(curve.r, X);
    BigInt y0 = big_int_sub(rX, X_plus_1_squared);

    BigInt y1 = big_int_add(rX, X_plus_1_squared);
    y1 = big_int_inverse(y1, curve.q);

    y0 = big_int_mul(y0, y1);
    BigInt y = big_int_mod(y0, curve.q); //  y = (rX − (1 + X)**2) / (rX + (1 + X)**2)

    CurvePoint r = {x, y};
    return r;
}

BigInt elligator_1_point_to_string(CurvePoint p, Curve curve)
{
    BigInt eta0 = big_int_sub(p.y, create_big_int(1));
    BigInt eta1 = big_int_add(p.y, create_big_int(1));
    eta1 = big_int_mul(create_big_int(2), eta1);
    eta1 = big_int_inverse(eta1, curve.q);
    eta0 = big_int_mul(eta0, eta1);
    BigInt eta = big_int_mod(eta0, curve.q); // η = (y-1)/(2(y+1))

    BigInt eta_r = big_int_mul(eta, curve.r);
    eta_r = big_int_add(create_big_int(1), eta_r);
    BigInt q_1 = big_int_add(curve.q, create_big_int(1));
    BigInt X0 = big_int_inverse(create_big_int(4), curve.q);
    q_1 = big_int_mul(q_1, X0);

    BigInt X1 = big_int_mul(eta_r, eta_r);
    X1 = big_int_sub(X1, create_big_int(1));
    X1 = big_int_pow(X1, q_1, curve.q);
    X1 = big_int_sub(X1, eta_r);
    BigInt X = big_int_mod(X1, curve.q); // X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)

    BigInt x0 = big_int_mul(create_big_int(2), curve.s);
    BigInt x1 = big_int_sub(curve.c, create_big_int(1));
    x0 = big_int_mul(x0, x1);
    BigInt x2 = chi(curve.c, curve.q);
    x0 = big_int_mul(x0, x2);
    BigInt x3 = big_int_inverse(curve.r, curve.q);
    x0 = big_int_mul(x0, x3);
    BigInt x = big_int_mod(x0, curve.q);  // x = 2s(c − 1)χ(c)/r

    BigInt z0 = big_int_sub(curve.c, create_big_int(1));
    z0 = big_int_mul(z0, curve.s);
    z0 = big_int_mul(z0, X);
    BigInt z1 = big_int_add(create_big_int(1), X);
    z0 = big_int_mul(z0, z1);
    z0 = big_int_mul(z0, x);
    BigInt z2 = big_int_mul(X, X);
    BigInt z3 = big_int_mul(curve.c, curve.c);
    z3 = big_int_inverse(z3, curve.q);
    z2 = big_int_add(z2, z3);
    z0 = big_int_mul(z0, z2);
    z0 = chi(z0, curve.q);
    BigInt z = big_int_mod(z0, curve.q);  // z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))

    BigInt u0 = big_int_mul(z, X);
    BigInt u = big_int_mod(u0, curve.q);  // u = zX

    BigInt t0 = big_int_sub(create_big_int(1), u);
    BigInt t1 = big_int_add(create_big_int(1), u);
    t1 = big_int_inverse(t1, curve.q);
    t0 = big_int_mul(t0, t1);
    BigInt t = big_int_mod(t0, curve.q);  // t = (1 − u)/(1 + u)

    return t;
}

Curve curve1174;
// curve1174.q = create_big_int(2 ** 251 - 9);
// curve1174.d = big_int_mod(create_big_int(-1174), curve1174.q);
// curve1174.s = create_big_int("1806494121122717992522804053500797229648438766985538871240722010849934886421");

// BigInt c0 = big_int_mul(curve1174.s, curve1174.s);
// c0 = big_int_inverse(c0, curve1174.q);
// c0 = big_int_mul(create_big_int(2), c0);
// c0 = big_int_mod(c0, curve1174.q);
// curve1174.c = c0;  // c = 2/(s**2);

// BigInt r0 = big_int_inverse(self.c, curve1174.q);
// r0 = big_int_add(curve1174.c, r0);
// r0 = big_int_mod(r0, curve1174.q);
// curve1174.r = r0;  // r = c + 1/c

int main()
{
    return 0;
}

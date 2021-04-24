#include "elligator.h"
#include "bigint.h"

CurvePoint elligator_1_string_to_point(BigInt *t, Curve curve)
{
    // TODO: assert t =/= +1, -1
    BigInt *u0 = big_int_sub(NULL, big_int_one, t);
    BigInt *u1 = big_int_add(NULL, big_int_one, t);
    u1 = big_int_inv(u1, u1, curve.q);
    BigInt *u = big_int_mul_mod(NULL, u0, u1, curve.q); // u = (1 − t) / (1 + t)

    // TODO: Optimization potential; reuse partial results for powers, instead
    // of doing it from scratch each time
    BigInt *v0 = big_int_pow(NULL, u, big_int_five, curve.q);
    BigInt *v1 = big_int_pow(NULL, curve.r, big_int_two, curve.q);
    v1 = big_int_sub(v1, v1, big_int_two);
    BigInt *v2 = big_int_pow(NULL, u, big_int_three, curve.q);
    v1 = big_int_mul_mod(v1, v1, v2, curve.q);
    v0 = big_int_add(v0, v0, v1);
    v0 = big_int_add(v0, v0, u);
    BigInt *v = big_int_mod(NULL, v0, curve.q);  // v = u**5 + (r**2 − 2)*u**3 + u

    BigInt *CHIV = big_int_chi(NULL, v, curve.q);

    BigInt *X = big_int_mul_mod(NULL, CHIV, u, curve.q);  // X = χ(v)u

    BigInt *Y0 = big_int_mul_mod(NULL, CHIV, v, curve.q);
    BigInt *Y1 = big_int_add(NULL, curve.q, big_int_one);
    BigInt *Y2 = big_int_inv(NULL, big_int_four, curve.q);
    Y1 = big_int_mul_mod(Y1, Y1, Y2, curve.q);
    Y0 = big_int_pow(Y0, Y0, Y1, curve.q);
    Y0 = big_int_mul_mod(Y0, Y0, CHIV, curve.q);

    BigInt *C2 = big_int_pow(NULL, curve.c, big_int_two, curve.q);
    C2 = big_int_inv(C2, C2, curve.q);
    BigInt *Y3 = big_int_pow(NULL, u, big_int_two, curve.q);
    Y3 = big_int_add(Y3, Y3, C2);
    Y3 = big_int_mod(Y3, Y3, curve.q);
    Y3 = big_int_chi(Y3, Y3, curve.q);

    BigInt *Y = big_int_mul_mod(Y0, Y0, Y3, curve.q);  // Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2)

    BigInt *X_plus_1 = big_int_add(NULL, big_int_one, X);
    BigInt *X_plus_1_squared = big_int_pow(NULL, X_plus_1, big_int_two, curve.q);

    BigInt *x0 = big_int_sub(NULL, curve.c, big_int_one);
    x0 = big_int_mul_mod(x0, x0, curve.s, curve.q);
    x0 = big_int_mul_mod(x0, x0, X, curve.q);
    x0 = big_int_mul_mod(x0, x0, X_plus_1, curve.q);
    BigInt *x1 = big_int_inv(NULL, Y, curve.q);
    BigInt *x = big_int_mul(NULL, x0, x1); // x = (c − 1)*s*X*(1 + X) / Y

    BigInt *rX = big_int_mul_mod(NULL, curve.r, X, curve.q);
    BigInt *y0 = big_int_sub(NULL, rX, X_plus_1_squared);

    BigInt *y1 = big_int_add(NULL, rX, X_plus_1_squared);
    y1 = big_int_inv(y1, y1, curve.q);

    BigInt *y = big_int_mul_mod(NULL, y0, y1, curve.q); //  y = (rX − (1 + X)**2) / (rX + (1 + X)**2)

    big_int_destroy(u0);
    big_int_destroy(u1);
    big_int_destroy(u);

    big_int_destroy(CHIV);

    big_int_destroy(X);

    big_int_destroy(Y0);
    big_int_destroy(Y1);
    big_int_destroy(Y2);
    big_int_destroy(Y3);

    big_int_destroy(C2);

    big_int_destroy(X_plus_1);
    big_int_destroy(X_plus_1_squared);

    big_int_destroy(x0);
    big_int_destroy(x1);

    big_int_destroy(rX);

    big_int_destroy(y0);
    big_int_destroy(y1);

    CurvePoint r = {x, y};
    return r;
}


BigInt *elligator_1_point_to_string(CurvePoint p, Curve curve)
{
    BigInt *eta0 = big_int_sub(NULL, p.y, big_int_one);
    BigInt *eta1 = big_int_add(NULL, p.y, big_int_one);
    eta1 = big_int_mul_mod(eta1, big_int_two, eta1, curve.q);
    eta1 = big_int_inv(eta1, eta1, curve.q);
    BigInt *eta = big_int_mul_mod(NULL, eta0, eta1, curve.q);  // η = (y-1)/(2(y+1))

    BigInt *eta_r = big_int_mul_mod(NULL, eta, curve.r, curve.q);
    eta_r = big_int_add(eta_r, big_int_one, eta_r);
    BigInt *q_1 = big_int_add(NULL, curve.q, big_int_one);
    // TODO: This is module order(q), which is q-1 if q is prime. Is this guaranteed?
    BigInt *X0 = big_int_sub(NULL, curve.q, big_int_one);
    X0 = big_int_inv(X0, big_int_four, X0);
    q_1 = big_int_mul_mod(q_1, q_1, X0, curve.q);

    BigInt *X1 = big_int_pow(NULL, eta_r, big_int_two, curve.q);
    X1 = big_int_sub(X1, X1, big_int_one);
    X1 = big_int_pow(X1, X1, q_1, curve.q);
    X1 = big_int_sub(X1, X1, eta_r);
    BigInt *X = big_int_mod(X1, X1, curve.q); // X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)

    // TODO: What happens if ηr =/= -2?
    // TODO: Shouldn't p.x already be this value, i.e., p.x = 2s(c − 1)χ(c)/r
    // if ηr =/= -2 and otherwise we just take the value of p.x?
    BigInt *x0 = big_int_mul_mod(NULL, big_int_two, curve.s, curve.q);
    BigInt *x1 = big_int_sub(NULL, curve.c, big_int_one);
    x0 = big_int_mul_mod(x0, x0, x1, curve.q);
    BigInt *x2 = big_int_chi(NULL, curve.c, curve.q);
    x0 = big_int_mul_mod(x0, x0, x2, curve.q);
    BigInt *x3 = big_int_inv(NULL, curve.r, curve.q);
    BigInt *x = big_int_mul_mod(NULL, x0, x3, curve.q); // x = 2s(c − 1)χ(c)/r

    BigInt *z0 = big_int_sub(NULL, curve.c, big_int_one);
    z0 = big_int_mul_mod(z0, z0, curve.s, curve.q);
    z0 = big_int_mul_mod(z0, z0, X, curve.q);
    BigInt *z1 = big_int_add(NULL, big_int_one, X);
    z0 = big_int_mul_mod(z0, z0, z1, curve.q);
    z0 = big_int_mul_mod(z0, z0, x, curve.q);
    BigInt *z2 = big_int_pow(NULL, X, big_int_two, curve.q);
    BigInt *z3 = big_int_pow(NULL, curve.c, big_int_two, curve.q);
    z3 = big_int_inv(z3, z3, curve.q);
    z2 = big_int_add(z2, z2, z3);
    z0 = big_int_mul_mod(z0, z0, z2, curve.q);
    z0 = big_int_chi(z0, z0, curve.q);
    BigInt *z = big_int_mod(NULL, z0, curve.q);  // z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))

    BigInt *u = big_int_mul_mod(NULL, z, X, curve.q); // u = zX

    big_int_destroy(u);

    BigInt *t0 = big_int_sub(NULL, big_int_one, u);
    BigInt *t1 = big_int_add(NULL, big_int_one, u);
    t1 = big_int_inv(t1, t1, curve.q);
    BigInt *t = big_int_mul_mod(NULL, t0, t1, curve.q); // t = (1 − u)/(1 + u)

    big_int_destroy(eta0);
    big_int_destroy(eta1);
    big_int_destroy(eta);

    big_int_destroy(eta_r);
    big_int_destroy(q_1);
    big_int_destroy(X0);

    big_int_destroy(X1);
    big_int_destroy(X);

    big_int_destroy(x0);
    big_int_destroy(x1);
    big_int_destroy(x2);
    big_int_destroy(x3);
    big_int_destroy(x);

    big_int_destroy(z0);
    big_int_destroy(z1);
    big_int_destroy(z2);
    big_int_destroy(z3);

    big_int_destroy(t0);
    big_int_destroy(t1);

    return t;
}

/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements the Elligator 1 mapping for Curve 1174 and its inverse.
 */

/*
* Includes
*/

#include "elligator.h"
#include "bigint.h"
#include "debug.h"


/**
* \brief Initialized Curve1174
*/
void init_curve1174(Curve *curve)
{
    // q = 2^251 - 9
    curve->q = big_int_create_from_hex(NULL,
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");

    curve->d = big_int_create_from_chunk(NULL, 1174, 1);
    curve->d = big_int_mod(curve->d, curve->d, curve->q);

    curve->s = big_int_create_from_hex(NULL,
        "3FE707F0D7004FD334EE813A5F1A74AB2449139C82C39D84A09AE74CC78C615");

    // c = 2 / s^2
    curve->c = big_int_mul_mod(NULL, curve->s, curve->s, curve->q);
    big_int_div_mod(curve->c, big_int_two, curve->c, curve->q);

    // r = c + 1/c
    curve->r = big_int_inv(NULL, curve->c, curve->q);
    big_int_add_mod(curve->r, curve->c, curve->r, curve->q);
}


/**
* \brief Free elliptic curve
*/
void free_curve(Curve *curve)
{
    big_int_destroy(curve->q);
    big_int_destroy(curve->d);
    big_int_destroy(curve->s);
    big_int_destroy(curve->c);
    big_int_destroy(curve->r);
}


/**
* \brief Free elliptic curve point
*/
void free_curve_point(CurvePoint *point)
{
    big_int_destroy(point->x);
    big_int_destroy(point->y);
}

/**
 * \brief Maps a random string (interpreted as big integer) to a point on the
 *        given curve.
 *
 * \param t Integer in range [0, (q-1)/2]
 * \param curve Curve satisfying the properties needed for Elligator one (e.g. Curve1174)
 */
CurvePoint elligator_1_string_to_point(BigInt *t, Curve curve)
{
    BigInt *q_half,
           *u, *u0, *u1,
           *v, *v0, *v1, *v2,
           *CHIV,
           *X, *X_plus_1, *X_plus_1_squared,
           *Y, *Y0, *Y1, *Y2, *Y3, *C2,
           *x, *x0, *rX,
           *y, *y0, *y1;

    // Enforce correct input range
    q_half = big_int_sub(NULL, curve.q, big_int_one);
    big_int_div(q_half, q_half, big_int_two);

    if (big_int_compare(t, big_int_zero) == -1 || big_int_compare(t, q_half) == 1)
        FATAL("Invalid input value for Elligator 1: t must be in the range [0, (q-1)/2]\n");

    if (big_int_compare(t, big_int_one) == 0)
    {
        x = big_int_create_from_chunk(NULL, 0, 0);
        y = big_int_create_from_chunk(NULL, 1, 0);
    }
    else {
        u0 = big_int_sub(NULL, big_int_one, t);
        u1 = big_int_add(NULL, big_int_one, t);
        u = big_int_div_mod(NULL, u0, u1, curve.q); // u = (1 − t) / (1 + t)

        // TODO: Optimization potential; reuse partial results for powers, instead
        // of doing it from scratch each time
        v0 = big_int_pow(NULL, u, big_int_five, curve.q);
        v1 = big_int_pow(NULL, curve.r, big_int_two, curve.q);
        v1 = big_int_sub(v1, v1, big_int_two);
        v2 = big_int_pow(NULL, u, big_int_three, curve.q);
        v1 = big_int_mul_mod(v1, v1, v2, curve.q);
        v0 = big_int_add(v0, v0, v1);
        v0 = big_int_add(v0, v0, u);
        v = big_int_mod(NULL, v0, curve.q);  // v = u**5 + (r**2 − 2)*u**3 + u

        CHIV = big_int_chi(NULL, v, curve.q);

        X = big_int_mul_mod(NULL, CHIV, u, curve.q);  // X = χ(v)u

        Y0 = big_int_mul_mod(NULL, CHIV, v, curve.q);
        Y1 = big_int_add(NULL, curve.q, big_int_one);
        Y2 = big_int_div(NULL, Y1, big_int_four);
        Y0 = big_int_pow(Y0, Y0, Y2, curve.q);
        Y0 = big_int_mul_mod(Y0, Y0, CHIV, curve.q);

        C2 = big_int_pow(NULL, curve.c, big_int_two, curve.q);
        C2 = big_int_inv(C2, C2, curve.q);

        Y3 = big_int_pow(NULL, u, big_int_two, curve.q);
        Y3 = big_int_add_mod(Y3, Y3, C2, curve.q);
        Y3 = big_int_chi(Y3, Y3, curve.q);
        Y = big_int_mul_mod(Y0, Y0, Y3, curve.q);  // Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2)

        X_plus_1 = big_int_add(NULL, big_int_one, X);
        X_plus_1_squared = big_int_pow(NULL, X_plus_1, big_int_two, curve.q);

        x0 = big_int_sub(NULL, curve.c, big_int_one);
        x0 = big_int_mul_mod(x0, x0, curve.s, curve.q);
        x0 = big_int_mul_mod(x0, x0, X, curve.q);
        x0 = big_int_mul_mod(x0, x0, X_plus_1, curve.q);
        x = big_int_div_mod(NULL, x0, Y, curve.q); // x = (c − 1)*s*X*(1 + X) / Y

        rX = big_int_mul_mod(NULL, curve.r, X, curve.q);
        y0 = big_int_sub(NULL, rX, X_plus_1_squared);

        y1 = big_int_add(NULL, rX, X_plus_1_squared);
        y = big_int_div_mod(NULL, y0, y1, curve.q); //  y = (rX − (1 + X)**2) / (rX + (1 + X)**2)

        big_int_destroy(u0);
        big_int_destroy(u1);
        big_int_destroy(u);

        big_int_destroy(v);
        big_int_destroy(v0);
        big_int_destroy(v1);
        big_int_destroy(v2);

        big_int_destroy(CHIV);

        big_int_destroy(X);
        big_int_destroy(X_plus_1);
        big_int_destroy(X_plus_1_squared);

        big_int_destroy(Y0);
        big_int_destroy(Y1);
        big_int_destroy(Y2);
        big_int_destroy(Y3);
        big_int_destroy(C2);

        big_int_destroy(x0);
        big_int_destroy(rX);

        big_int_destroy(y0);
        big_int_destroy(y1);
    }

    big_int_destroy(q_half);

    CurvePoint r = {x, y};
    return r;
}

/**
 * \brief Maps a point on the given curve back to a random value in the range
 *        [0, (q-1)/2]
 *
 * Prerequisites:
 *  - the given point is on the curve
 *
 * \param p Point on the given curve (with x, y coordinates)
 * \param curve Curve satisfying the properties needed for Elligator one (e.g. Curve1174)
 */
BigInt *elligator_1_point_to_string(CurvePoint p, Curve curve)
{
    BigInt *eta, *eta0, *eta1,
           *eta_r, *q_exp,
           *X, *X0,
           *z, *z0, *z1, *z2, *z3,
           *u,
           *t, *t0, *t1,
           *q_half;

    eta0 = big_int_sub(NULL, p.y, big_int_one);
    eta1 = big_int_add(NULL, p.y, big_int_one);
    eta1 = big_int_mul_mod(eta1, big_int_two, eta1, curve.q);
    eta = big_int_div_mod(eta0, eta0, eta1, curve.q);  // η = (y-1)/(2(y+1))

    eta_r = big_int_mul_mod(NULL, eta, curve.r, curve.q);
    eta_r = big_int_add(eta_r, big_int_one, eta_r);
    q_exp = big_int_add(NULL, curve.q, big_int_one);
    q_exp = big_int_div(q_exp, q_exp, big_int_four);

    X0 = big_int_pow(NULL, eta_r, big_int_two, curve.q);
    X0 = big_int_sub(X0, X0, big_int_one);
    X0 = big_int_pow(X0, X0, q_exp, curve.q);
    X = big_int_sub_mod(X0, X0, eta_r, curve.q); // X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)

    // TODO: We might want to consider the case where ηr == -2, where we have:
    //       x = 2s(c − 1)χ(c)/r
    //       But as long as we don't compress points, it's anyway much cheaper
    //       to just use the given x from the point.

    z0 = big_int_sub(NULL, curve.c, big_int_one);
    z0 = big_int_mul_mod(z0, z0, curve.s, curve.q);
    z0 = big_int_mul_mod(z0, z0, X, curve.q);
    z1 = big_int_add(NULL, big_int_one, X);
    z0 = big_int_mul_mod(z0, z0, z1, curve.q);
    z0 = big_int_mul_mod(z0, z0, p.x, curve.q);
    z2 = big_int_pow(NULL, X, big_int_two, curve.q);
    z3 = big_int_pow(NULL, curve.c, big_int_two, curve.q);
    z3 = big_int_inv(z3, z3, curve.q);
    z2 = big_int_add(z2, z2, z3);
    z0 = big_int_mul_mod(z0, z0, z2, curve.q);
    z0 = big_int_chi(z0, z0, curve.q);
    z = big_int_mod(z0, z0, curve.q);  // z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))

    u = big_int_mul_mod(NULL, z, X, curve.q); // u = zX

    t0 = big_int_sub(NULL, big_int_one, u);
    t1 = big_int_add(NULL, big_int_one, u);
    t = big_int_div_mod(t0, t0, t1, curve.q); // t = (1 − u)/(1 + u)

    // TODO: Optimization potential, subtraction is not necessary. Use SRL and
    //       shift those bits away!
    q_half = big_int_sub(NULL, curve.q, big_int_one);
    q_half = big_int_div(q_half, q_half, big_int_two); // q_half = (q - 1)/2

    if (big_int_compare(t, q_half) == 1) {
        t = big_int_neg(t, t);
        t = big_int_mod(t, t, curve.q);
    }

    big_int_destroy(eta0);
    big_int_destroy(eta1);

    big_int_destroy(eta_r);
    big_int_destroy(q_exp);

    big_int_destroy(X0);

    big_int_destroy(z0);
    big_int_destroy(z1);
    big_int_destroy(z2);
    big_int_destroy(z3);

    big_int_destroy(u);

    big_int_destroy(t1);

    return t;
}

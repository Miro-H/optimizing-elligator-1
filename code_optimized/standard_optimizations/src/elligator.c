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
    big_int_create_from_hex(&(curve->q),
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");

    // d = -1174
    big_int_create_from_hex(&(curve->d),
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB61");

    big_int_create_from_hex(&(curve->s),
        "3FE707F0D7004FD334EE813A5F1A74AB2449139C82C39D84A09AE74CC78C615");

    // c = 2 / s^2
    big_int_create_from_hex(&(curve->c),
        "4D1A3398ED42CEEB451D20824CA9CB49B69EF546BD7E6546AEF19AF1F9E49E1");

    // r = c + 1/c
    big_int_create_from_hex(&(curve->r),
        "6006FBDA7649C433816B286006FBDA7649C433816B286006FBDA7649C43383");
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
    BIG_INT_DEFINE_PTR(q_half);
    BIG_INT_DEFINE_PTR(u);
    BIG_INT_DEFINE_PTR(v);
    BIG_INT_DEFINE_PTR(CHIV);

    BIG_INT_DEFINE_PTR(X);
    BIG_INT_DEFINE_PTR(X_plus_1);
    BIG_INT_DEFINE_PTR(X_plus_1_squared);

    BIG_INT_DEFINE_PTR(rX);
    BIG_INT_DEFINE_PTR(Y);

    BIG_INT_DEFINE_PTR(x);
    BIG_INT_DEFINE_PTR(y);

    BIG_INT_DEFINE_PTR(tmp_0);
    BIG_INT_DEFINE_PTR(tmp_1);
    BIG_INT_DEFINE_PTR(tmp_2);
    BIG_INT_DEFINE_PTR(tmp_3);
    BIG_INT_DEFINE_PTR(tmp_4);

    // Enforce correct input range
    big_int_sub(tmp_0, &(curve.q), big_int_one);
    big_int_div(q_half, tmp_0, big_int_two);

    if (big_int_compare(t, big_int_zero) == -1 || big_int_compare(t, q_half) == 1)
        FATAL("Invalid input value for Elligator 1: t must be in the range [0, (q-1)/2]\n");

    if (big_int_compare(t, big_int_one) == 0)
    {
        big_int_create_from_chunk(x, 0, 0);
        big_int_create_from_chunk(y, 1, 0);
    }
    else {

        big_int_sub(tmp_0, big_int_one, t);
        big_int_add(tmp_1, big_int_one, t);
        big_int_div_mod(u, tmp_0, tmp_1, &(curve.q)); // u = (1 − t) / (1 + t)

        // TODO: Optimization potential; reuse partial results for powers, instead
        // of doing it from scratch each time

        big_int_pow(tmp_0, u, big_int_five, &(curve.q));
        big_int_pow(tmp_1, &(curve.r), big_int_two, &(curve.q));
        big_int_sub(tmp_2, tmp_1, big_int_two);
        big_int_pow(tmp_3, u, big_int_three, &(curve.q));
        big_int_mul_mod(tmp_4, tmp_2, tmp_3, &(curve.q));
        big_int_add(tmp_1, tmp_0, tmp_4);
        big_int_add(tmp_2, tmp_1, u);
        big_int_mod(v, tmp_2, &(curve.q));  // v = u**5 + (r**2 − 2)*u**3 + u

        big_int_chi(CHIV, v, &(curve.q));

        big_int_mul_mod(X, CHIV, u, &(curve.q));  // X = χ(v)u

        big_int_mul_mod(tmp_0, CHIV, v, &(curve.q));
        big_int_add(tmp_1, &(curve.q), big_int_one);
        big_int_div(tmp_2, tmp_1, big_int_four);
        big_int_pow(tmp_1, tmp_0, tmp_2, &(curve.q));
        big_int_mul_mod(tmp_0, tmp_1, CHIV, &(curve.q));

        big_int_pow(tmp_1, &(curve.c), big_int_two, &(curve.q));
        big_int_inv(tmp_3, tmp_1, &(curve.q));

        big_int_pow(tmp_1, u, big_int_two, &(curve.q));
        big_int_add_mod(tmp_2, tmp_1, tmp_3, &(curve.q));
        big_int_chi(tmp_3, tmp_2, &(curve.q));
        big_int_mul_mod(Y, tmp_0, tmp_3, &(curve.q));  // Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2)

        big_int_add(X_plus_1, big_int_one, X);
        big_int_pow(X_plus_1_squared, X_plus_1, big_int_two, &(curve.q));

        big_int_sub(tmp_0, &(curve.c), big_int_one);
        big_int_mul_mod(tmp_1, tmp_0, &(curve.s), &(curve.q));
        big_int_mul_mod(tmp_0, tmp_1, X, &(curve.q));
        big_int_mul_mod(tmp_1, tmp_0, X_plus_1, &(curve.q));
        big_int_div_mod(x, tmp_1, Y, &(curve.q)); // x = (c − 1)*s*X*(1 + X) / Y

        big_int_mul_mod(rX, &(curve.r), X, &(curve.q));
        big_int_sub(tmp_0, rX, X_plus_1_squared);

        big_int_add(tmp_1, rX, X_plus_1_squared);
        big_int_div_mod(y, tmp_0, tmp_1, &(curve.q)); //  y = (rX − (1 + X)**2) / (rX + (1 + X)**2)

    }

    CurvePoint r = {*x, *y};
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
    BIG_INT_DEFINE_PTR(X);
    BIG_INT_DEFINE_PTR(z);
    BIG_INT_DEFINE_PTR(u);
    BIG_INT_DEFINE_PTR(t);
    BIG_INT_DEFINE_PTR(q_half);

    BIG_INT_DEFINE_PTR(tmp_0);
    BIG_INT_DEFINE_PTR(tmp_1);
    BIG_INT_DEFINE_PTR(tmp_2);
    BIG_INT_DEFINE_PTR(tmp_3);

    big_int_sub(tmp_0, &(p.y), big_int_one);
    big_int_add(tmp_1, &(p.y), big_int_one);
    big_int_mul_mod(tmp_2, big_int_two, tmp_1, &(curve.q));
    big_int_div_mod(tmp_1, tmp_0, tmp_2, &(curve.q));  // η = (y-1)/(2(y+1))

    big_int_mul_mod(tmp_0, tmp_1, &(curve.r), &(curve.q));
    big_int_add(tmp_1, big_int_one, tmp_0);
    big_int_add(tmp_0, &(curve.q), big_int_one);
    big_int_div(tmp_2, tmp_0, big_int_four);

    big_int_pow(tmp_3, tmp_1, big_int_two, &(curve.q));
    big_int_sub(tmp_0, tmp_3, big_int_one);
    big_int_pow(tmp_3, tmp_0, tmp_2, &(curve.q));
    X = big_int_sub_mod(X, tmp_3, tmp_1, &(curve.q)); // X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)

    // TODO: We might want to consider the case where ηr == -2, where we have:
    //       x = 2s(c − 1)χ(c)/r
    //       But as long as we don't compress points, it's anyway much cheaper
    //       to just use the given x from the point.

    big_int_sub(tmp_0, &(curve.c), big_int_one);
    big_int_mul_mod(tmp_1, tmp_0, &(curve.s), &(curve.q));
    big_int_mul_mod(tmp_0, tmp_1, X, &(curve.q));
    big_int_add(tmp_1, big_int_one, X);
    big_int_mul_mod(tmp_2, tmp_0, tmp_1, &(curve.q));
    big_int_mul_mod(tmp_0, tmp_2, &(p.x), &(curve.q));
    big_int_pow(tmp_1, X, big_int_two, &(curve.q));
    big_int_pow(tmp_2, &(curve.c), big_int_two, &(curve.q));
    big_int_inv(tmp_3, tmp_2, &(curve.q));
    big_int_add(tmp_2, tmp_1, tmp_3);
    big_int_mul_mod(tmp_1, tmp_0, tmp_2, &(curve.q));
    big_int_chi(tmp_0, tmp_1, &(curve.q));
    big_int_mod(z, tmp_0, &(curve.q));  // z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))

    big_int_mul_mod(u, z, X, &(curve.q)); // u = zX

    big_int_sub(tmp_0, big_int_one, u);
    big_int_add(tmp_1, big_int_one, u);
    big_int_div_mod(t, tmp_0, tmp_1, &(curve.q)); // t = (1 − u)/(1 + u)

    // TODO: Optimization potential, subtraction is not necessary. Use SRL and
    //       shift those bits away!
    big_int_sub(tmp_0, &(curve.q), big_int_one);
    big_int_div(q_half, tmp_0, big_int_two); // q_half = (q - 1)/2

    if (big_int_compare(t, q_half) == 1) {
        big_int_neg(tmp_0, t);
        big_int_mod(t, tmp_0, &(curve.q));
    }

    return t;
}

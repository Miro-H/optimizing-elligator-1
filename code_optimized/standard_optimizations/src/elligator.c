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
#include "bigint_curve1174.h"
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

    // c - 1
    big_int_create_from_hex(&(curve->c_minus_1),
        "4D1A3398ED42CEEB451D20824CA9CB49B69EF546BD7E6546AEF19AF1F9E49E0");

     // (c - 1) * s
    big_int_create_from_hex(&(curve->c_minus_1_s),
        "67897DC1C0CCC95F80AC25CFB7DBA3F085E0A97C32385BA7DC4079961D335B1");

    // 1 / c^2
    big_int_create_from_hex(&(curve->c_squared_inverse),
        "771F18AED833220B34B0FDADEFE83B1C247BCCD1D7983A9438B412D3C3700BA");

    // r = c + 1/c
    big_int_create_from_hex(&(curve->r),
        "6006FBDA7649C433816B286006FBDA7649C433816B286006FBDA7649C43383");

    // r^2 - 2
    big_int_create_from_hex(&(curve->r_squared_minus_two),
        "1C9C4399A2B9D9AF7AA044B36AB903EB9E91E21C901E4A392E6E34834A14BE8");
}

// === === === === === === === === === === === === === === === === === === ===

/**
 * \brief Maps a random string (interpreted as big integer) to a point on the
 *        given curve.
 *
 * \param t Integer in range [0, (q-1)/2] does no longer do sanity checks
 * \param curve Curve satisfying the properties needed for Elligator one (e.g. Curve1174)
 */
CurvePoint *elligator_1_string_to_point(CurvePoint *r, BigInt *t, Curve curve)
{
    if (t->size == 1 && t->chunks[0] == (dbl_chunk_size_t) 1)
    {
        big_int_create_from_chunk(&(r->x), 0, 0);
        big_int_create_from_chunk(&(r->y), 1, 0);
        return r;
    }

    BigInt *X;
    BigInt *Y;
    BigInt *u_3;
    BigInt *u_5;

    BIG_INT_DEFINE_PTR(u);
    BIG_INT_DEFINE_PTR(u_2);

    BIG_INT_DEFINE_PTR(v);

    int8_t chiv, chi_2;

    BIG_INT_DEFINE_PTR(X_plus_1_squared);

    BIG_INT_DEFINE_PTR(tmp_0);
    BIG_INT_DEFINE_PTR(tmp_1);
    BIG_INT_DEFINE_PTR(tmp_2);
    BIG_INT_DEFINE_PTR(tmp_3);

    BIG_INT_DEFINE_FROM_CHUNK(tmp_pow_res, 0, 1);

    big_int_sub(tmp_0, big_int_one, t);
    big_int_add(tmp_1, big_int_one, t);
    big_int_div_mod(u, tmp_0, tmp_1, &(curve.q)); // u = (1 − t) / (1 + t)

    big_int_curve1174_square_mod(u_2, u);
    u_3 = tmp_3;
    big_int_curve1174_mul_mod(u_3, u_2, u); // u_3 = u^3

    u_5 = tmp_1;

    big_int_curve1174_mul_mod(u_5, u_3, u_2); // u_5 = u^5
    big_int_curve1174_mul_mod(tmp_0, &(curve.r_squared_minus_two), u_3); // (r^2 − 2)*u^3

    big_int_add(v, u_5, u); // u^5 + u
    big_int_add(v, v, tmp_0); // v = u^5 + (r^2 − 2)*u^3 + u

    // Preserve value for later
    big_int_copy(tmp_0, v);

    // this call modifies v inplace
    chiv = big_int_curve1174_chi(v);

    X = u;
    X->sign = X->sign ^ chiv; // X = χ(v)u

    tmp_0->sign = tmp_0->sign ^ chiv; // χ(v)v

    big_int_curve1174_pow_q_p1_d4(tmp_pow_res, tmp_0); // (χ(v)v)^((q + 1) / 4)
    tmp_pow_res->sign = tmp_pow_res->sign ^ chiv; // (χ(v)v)^((q + 1) / 4)χ(v)

    big_int_add(tmp_2, u_2, &(curve.c_squared_inverse)); // u^2 + 1 / c^2
    chi_2 = big_int_curve1174_chi(tmp_2); // χ(u^2 + 1 / c^2)

    Y = tmp_pow_res;
    Y->sign = Y->sign ^ chi_2; // Y = (χ(v)v)^((q + 1) / 4)χ(v)χ(u^2 + 1 / c^2)


    big_int_add(tmp_0, big_int_one, X); // X+1
    big_int_curve1174_square_mod(X_plus_1_squared, tmp_0); // (X+1)^2

    big_int_curve1174_mul_mod(tmp_1, &(curve.c_minus_1_s), X); // (c - 1) * s * X
    big_int_curve1174_mul_mod(tmp_2, tmp_1, tmp_0); // (c - 1) * s * X * (1+X)
    big_int_curve1174_div_mod(&(r->x), tmp_2, Y); // x = (c − 1) * s * X * (1 + X) / Y

    big_int_curve1174_mul_mod(tmp_2, &(curve.r), X); // rX
    big_int_sub(tmp_0, tmp_2, X_plus_1_squared); // rX - (1 + X)^2

    big_int_add(tmp_1, tmp_2, X_plus_1_squared); // rX + (1 + X)^2
    big_int_curve1174_div_mod(&(r->y), tmp_0, tmp_1); //  y = (rX − (1 + X)^2) / (rX + (1 + X)^2)

    return r;
}



// === === === === === === === === === === === === === === === === === === ===

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
BigInt *elligator_1_point_to_string(BigInt *t, CurvePoint p, Curve curve)
{
    // Special case (x, y) = (0, 1) maps to 1
    // After this, z != 0, so the input of χ is not 0 and thus returns ±1
    if (p.x.size ==  1
        && p.y.size ==  1
        && p.x.chunks[0] == 0
        && p.y.chunks[0] == 1)
    {
        big_int_create_from_chunk(t, 1, 0);
        return t;
    }
    
    BIG_INT_DEFINE_PTR(X);
    //BIG_INT_DEFINE_PTR(eta);

    BigInt *u;

    int8_t z;

    BIG_INT_DEFINE_PTR(tmp_0);
    BIG_INT_DEFINE_PTR(tmp_1);
    BIG_INT_DEFINE_PTR(tmp_2);
    //BIG_INT_DEFINE_PTR(tmp_3);

    BIG_INT_DEFINE_FROM_CHUNK(tmp_pow_res, 0, 1);

    big_int_sub(tmp_0, &(p.y), big_int_one); // y - 1
    big_int_add(tmp_1, &(p.y), big_int_one); // y + 1
    big_int_sll_small(tmp_2, tmp_1, 1); // 2 * (y + 1)
    big_int_curve1174_div_mod(tmp_1, tmp_0, tmp_2); // η = (y-1)/(2 * (y + 1))

    big_int_curve1174_mul_mod(tmp_0, tmp_1, &(curve.r));
    big_int_add(tmp_1, big_int_one, tmp_0); // 1 + ηr

    big_int_curve1174_square_mod(tmp_2, tmp_1); // (1 + ηr)^2

    big_int_sub(tmp_0, tmp_2, big_int_one); // (1 + η  * r)^2 - 1

    big_int_curve1174_pow_q_p1_d4(tmp_pow_res, tmp_0); // ((1 + ηr)^2 - 1)^((q + 1) / 4)

    big_int_curve1174_sub_mod(X, tmp_pow_res, tmp_1); // X = −(1 + ηr) + ((1 + ηr)^2 − 1)^((q + 1) / 4)

    big_int_curve1174_mul_mod(tmp_0, &(curve.c_minus_1_s), X); // (c - 1)sX
    big_int_add(tmp_1, big_int_one, X); // 1 + X
    big_int_curve1174_mul_mod(tmp_2, tmp_0, tmp_1); // (c - 1)sX(1 + X)
    big_int_curve1174_mul_mod(tmp_0, tmp_2, &(p.x)); // (c - 1)sX(1 + X)x
    big_int_curve1174_square_mod(tmp_1, X); // X^2

    big_int_add(tmp_2, tmp_1, &(curve.c_squared_inverse)); // X^2 + 1/c^2
    big_int_curve1174_mul_mod(tmp_1, tmp_0, tmp_2); // (c - 1)sX(1 + X)x(X^2 + 1/c^2)

    // this call modifies tmp_1 inplace
    z = big_int_curve1174_chi(tmp_1); // z = χ((c - 1)sX(1 + X)x(X^2 + 1/c^2))

    u = X;
    u->sign = u->sign ^ z; // u = zX

    big_int_sub(tmp_0, big_int_one, u); // 1 - u
    big_int_add(tmp_1, big_int_one, u); // 1 + u
    big_int_curve1174_div_mod(t, tmp_0, tmp_1); // t = (1 − u)/(1 + u)

    if (big_int_curve1174_gt_q_m1_d2(t))
        big_int_sub(t, &(curve.q), t);

    return t;
}

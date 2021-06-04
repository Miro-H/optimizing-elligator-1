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

#include "gmp_elligator.h"
#include "debug.h"

#include <gmp.h>

/**
 * \brief Calculate the Chi function chi(t) = t**((q-1)/2) mod q
 * \returns 0 if t = 0, 1 if t is a non-zero square, -1 otherwise
 */
void mpz_chi(mpz_t r, mpz_t t, mpz_t q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CHI);

    // BigInt *e
    mpz_t e;
    mpz_t e_minus_1;

    mpz_init(e);
    mpz_init(e_minus_1);

    // Check assumption that q is odd (otherwise our chi function might need to
    // be changed)
    //if (!big_int_is_odd(q))
    if (mpz_divisible_ui_p(q, 2))
        FATAL("Chi function implementation only supports odd q (actually prime q)!");

    if (!r)
    {
        //r = big_int_alloc(BIGINT_FIXED_SIZE);
        mpz_init(r);
    }

    //if (big_int_compare(t, big_int_zero) == 0)
    if (mpz_cmp_ui(t, 0) == 0)
    {
        //return big_int_create_from_chunk(r, 0, 0);
        mpz_set_ui(r, 0);
        mpz_clear(e);
        mpz_clear(e_minus_1);
        return;
    }

    //e = big_int_duplicate(q);
    mpz_set(e, q); // 7
    //big_int_srl_small(e, big_int_sub(e, e, big_int_one), 1);
    mpz_sub_ui(e_minus_1, e, 1); // 7
    mpz_fdiv_q_2exp(e, e_minus_1, 1);

    //big_int_pow(r, t, e, q);
    mpz_powm(r, t, e, q);

    //big_int_destroy(e);
    mpz_clear(e);
    mpz_clear(e_minus_1);

    //if (!big_int_compare(r, big_int_zero) || !big_int_compare(r, big_int_one))
    if (!mpz_cmp_ui(r, 0) || !mpz_cmp_ui(r, 1))
    {
        //return r;
        return;
    }
    //return big_int_create_from_chunk(r, 1, 1); // r = -1
    mpz_set_si(r, -1);
    return;
}

/**
* \brief Initialized Curve1174
*/
void gmp_init_curve1174(GMP_Curve *curve)
{
    mpz_t gmp_two, inverse;

    mpz_init(gmp_two);
    mpz_init(curve->q);
    mpz_init(curve->d);
    mpz_init(curve->s);
    mpz_init(curve->c);
    mpz_init(curve->r);
    mpz_init(inverse);

    mpz_set_si(gmp_two, 2);

    // q = 2^251 - 9
    //curve->q = big_int_create_from_hex(NULL,
        //"7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    mpz_set_str(curve->q, 
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7", 16);

    //curve->d = big_int_create_from_chunk(NULL, 1174, 1);
    mpz_set_si(curve->d, -1174);
    //curve->d = big_int_mod(curve->d, curve->d, curve->q);
    mpz_mod(curve->d, curve->d, curve->q);

    //curve->s = big_int_create_from_hex(NULL,
        //"3FE707F0D7004FD334EE813A5F1A74AB2449139C82C39D84A09AE74CC78C615");
    mpz_set_str(curve->s,
        "3FE707F0D7004FD334EE813A5F1A74AB2449139C82C39D84A09AE74CC78C615", 16);

    // c = 2 / s^2
    //curve->c = big_int_mul_mod(NULL, curve->s, curve->s, curve->q);
    mpz_mul(curve->c, curve->s, curve->s);
    mpz_mod(curve->c, curve->c, curve->q);
    //big_int_div_mod(curve->c, big_int_two, curve->c, curve->q);
    mpz_invert(inverse, curve->c, curve->q);
    mpz_mul_ui(curve->c, inverse, 2);
    mpz_mod(curve->c, curve->c, curve->q);

    // r = c + 1/c
    //curve->r = big_int_inv(NULL, curve->c, curve->q);
    mpz_invert(curve->r, curve->c, curve->q);
    //big_int_add_mod(curve->r, curve->c, curve->r, curve->q);
    mpz_add(curve->r, curve->c, curve->r);
    mpz_mod(curve->r, curve->r, curve->q);
}


/**
* \brief Free elliptic curve
*/
void gmp_free_curve(GMP_Curve *curve)
{
    //big_int_destroy(curve->q);
    mpz_clear(curve->q);
    //big_int_destroy(curve->d);
    mpz_clear(curve->d);
    //big_int_destroy(curve->s);
    mpz_clear(curve->s);
    //big_int_destroy(curve->c);
    mpz_clear(curve->c);
    //big_int_destroy(curve->r);
    mpz_clear(curve->r);
}


/**
* \brief Free elliptic curve point
*/
void gmp_free_curve_point(GMP_CurvePoint *point)
{
    //big_int_destroy(point->x);
    mpz_clear(point->x);
    //big_int_destroy(point->y);
    mpz_clear(point->y);
}

/**
 * \brief Maps a random string (interpreted an mpz_t) to a point on the
 *        given curve.
 *
 * \param t Integer in range [0, (q-1)/2]
 * \param curve Curve satisfying the properties needed for Elligator one (e.g. Curve1174)
 */
GMP_CurvePoint *gmp_elligator_1_string_to_point(GMP_CurvePoint *r, mpz_t t, GMP_Curve curve)
{
    //BigInt *q_half,
           //*u, *u0, *u1,
           //*v, *v0, *v1, *v2,
           //*CHIV,
           //*X, *X_plus_1, *X_plus_1_squared,
           //*Y, *Y0, *Y1, *Y2, *Y3, *C2,
           //*x, *x0, *rX,
           //*y, *y0, *y1;
    mpz_t q_half,
          u, u0, u1,
          v, v0, v1, v2,
          CHIV,
          X, X_plus_1, X_plus_1_squared,
          Y, Y0, Y1, Y2, Y3, C2,
          x, x0, rX,
          y, y0, y1,
          inverse;

    mpz_init(q_half);
    mpz_init(u);
    mpz_init(u0);
    mpz_init(u1);
    mpz_init(v);
    mpz_init(v0);
    mpz_init(v1);
    mpz_init(v2);
    mpz_init(CHIV);
    mpz_init(X);
    mpz_init(X_plus_1);
    mpz_init(X_plus_1_squared);
    mpz_init(Y);
    mpz_init(Y0);
    mpz_init(Y1);
    mpz_init(Y2);
    mpz_init(Y3);
    mpz_init(C2);
    mpz_init(x);
    mpz_init(x0);
    mpz_init(rX);
    mpz_init(y);
    mpz_init(y0);
    mpz_init(y1);
    mpz_init(inverse);

    // Enforce correct input range
    //q_half = big_int_sub(NULL, curve.q, big_int_one);
    mpz_sub_ui(q_half, curve.q, 1);
    //big_int_div(q_half, q_half, big_int_two);
    mpz_fdiv_q_ui(q_half, q_half, 2);

    //if (big_int_compare(t, big_int_zero) == -1 || big_int_compare(t, q_half) == 1)
    if (mpz_cmp_si(t, 0) == -1 || mpz_cmp(t, q_half) == 1)
        FATAL("Invalid input value for Elligator 1: t must be in the range [0, (q-1)/2]\n");

    //if (big_int_compare(t, big_int_one) == 0)
    if (mpz_cmp_si(t, 1) == 0)
    {
        //x = big_int_create_from_chunk(NULL, 0, 0);
        mpz_set_si(x, 0);
        //y = big_int_create_from_chunk(NULL, 1, 0);
        mpz_set_si(y, 1);
    }
    else {
        //u0 = big_int_sub(NULL, big_int_one, t);
        mpz_ui_sub(u0, 1, t); // u0 = -6
        //u1 = big_int_add(NULL, big_int_one, t);
        mpz_add_ui(u1, t, 1); // u1 = 8
        //u = big_int_div_mod(NULL, u0, u1, curve.q); // u = (1 − t) / (1 + t)
        //mpz_fdiv_q(u, u0, u1);
        //mpz_mod(u, u, curve.q);
        mpz_invert(inverse, u1, curve.q);

        mpz_mul(u, u0, inverse);
        //-0x5fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa
        mpz_mod(u, u, curve.q);
        //0x1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffd

        //v0 = big_int_pow(NULL, u, big_int_five, curve.q);
        mpz_powm_ui(v0, u, 5, curve.q);
        // 0x7c9fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7
        //v1 = big_int_pow(NULL, curve.r, big_int_two, curve.q);
        mpz_powm_ui(v1, curve.r, 2, curve.q);
        // 0x1c9c4399a2b9d9af7aa044b36ab903eb9e91e21c901e4a392e6e34834a14bea
        //v1 = big_int_sub(v1, v1, big_int_two);
        mpz_sub_ui(v1, v1, 2);
        // 1c9c4399a2b9d9af7aa044b36ab903eb9e91e21c901e4a392e6e34834a14be8
        //v2 = big_int_pow(NULL, u, big_int_three, curve.q);
        mpz_powm_ui(v2, u, 3, curve.q); // 0x79ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7
        //v1 = big_int_mul_mod(v1, v1, v2, curve.q);
        mpz_mul(v1, v1, v2);
        mpz_mod(v1, v1, curve.q); // 0x3ee137b2f599829f84463044ef9f258991a749bf33338afe06981d89cbf3f9
        //v0 = big_int_add(v0, v0, v1);
        mpz_add(v0, v0, v1); // 0x808e137b2f599829f84463044ef9f258991a749bf33338afe06981d89cbf3f0
        //v0 = big_int_add(v0, v0, u);
        mpz_add(v0, v0, u); // 0xa08e137b2f599829f84463044ef9f258991a749bf33338afe06981d89cbf3ed
        //v = big_int_mod(NULL, v0, curve.q);  // v = u**5 + (r**2 − 2)*u**3 + u
        mpz_mod(v, v0, curve.q); // 0x208e137b2f599829f84463044ef9f258991a749bf33338afe06981d89cbf3f6

        //CHIV = big_int_chi(NULL, v, curve.q);
        mpz_chi(CHIV, v, curve.q); // 1

        //X = big_int_mul_mod(NULL, CHIV, u, curve.q);  // X = χ(v)u
        mpz_mul(X, CHIV, u); // 0x1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffd
        mpz_mod(X, X, curve.q);

        //Y0 = big_int_mul_mod(NULL, CHIV, v, curve.q);
        mpz_mul(Y0, CHIV, v);
        mpz_mod(Y0, Y0, curve.q);
        //Y1 = big_int_add(NULL, curve.q, big_int_one);
        mpz_add_ui(Y1, curve.q, 1);
        //Y2 = big_int_div(NULL, Y1, big_int_four);
        mpz_fdiv_q_ui(Y2, Y1, 4);
        //Y0 = big_int_pow(Y0, Y0, Y2, curve.q);
        mpz_powm(Y0, Y0, Y2, curve.q);
        //Y0 = big_int_mul_mod(Y0, Y0, CHIV, curve.q);
        mpz_mul(Y0, Y0, CHIV);
        mpz_mod(Y0, Y0, curve.q);

        //C2 = big_int_pow(NULL, curve.c, big_int_two, curve.q);
        mpz_powm_ui(C2, curve.c, 2, curve.q);
        //C2 = big_int_inv(C2, C2, curve.q);
        mpz_invert(C2, C2, curve.q);

        //Y3 = big_int_pow(NULL, u, big_int_two, curve.q);
        mpz_powm_ui(Y3, u, 2, curve.q);
        //Y3 = big_int_add_mod(Y3, Y3, C2, curve.q);
        mpz_add(Y3, Y3, C2);
        mpz_mod(Y3, Y3, curve.q);
        //Y3 = big_int_chi(Y3, Y3, curve.q);
        mpz_chi(Y3, Y3, curve.q);
        //Y = big_int_mul_mod(Y0, Y0, Y3, curve.q);  // Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2)
        mpz_mul(Y, Y0, Y3);
        mpz_mod(Y, Y, curve.q);
        //mpz_set(Y0, Y);

        //X_plus_1 = big_int_add(NULL, big_int_one, X);
        mpz_add_ui(X_plus_1, X, 1);
        //X_plus_1_squared = big_int_pow(NULL, X_plus_1, big_int_two, curve.q);
        mpz_powm_ui(X_plus_1_squared, X_plus_1, 2, curve.q);

        //x0 = big_int_sub(NULL, curve.c, big_int_one);
        mpz_sub_ui(x0, curve.c, 1);
        //x0 = big_int_mul_mod(x0, x0, curve.s, curve.q);
        mpz_mul(x0, x0, curve.s);
        mpz_mod(x0, x0, curve.q);
        //x0 = big_int_mul_mod(x0, x0, X, curve.q);
        mpz_mul(x0, x0, X);
        mpz_mod(x0, x0, curve.q);
        //x0 = big_int_mul_mod(x0, x0, X_plus_1, curve.q);
        mpz_mul(x0, x0, X_plus_1);
        mpz_mod(x0, x0, curve.q);
        //x = big_int_div_mod(NULL, x0, Y, curve.q); // x = (c − 1)*s*X*(1 + X) / Y
        //mpz_fdiv_q(x, x0, Y);
        //mpz_mod(x, x, curve.q);
        mpz_invert(inverse, Y, curve.q);
        mpz_mul(x, x0, inverse);
        mpz_mod(x, x, curve.q);

        //rX = big_int_mul_mod(NULL, curve.r, X, curve.q);
        mpz_mul(rX, curve.r, X);
        mpz_mod(rX, rX, curve.q);
        //y0 = big_int_sub(NULL, rX, X_plus_1_squared);
        mpz_sub(y0, rX, X_plus_1_squared);

        //y1 = big_int_add(NULL, rX, X_plus_1_squared);
        mpz_add(y1, rX, X_plus_1_squared);
        //y = big_int_div_mod(NULL, y0, y1, curve.q); //  y = (rX − (1 + X)**2) / (rX + (1 + X)**2)
        //mpz_fdiv_q(y, y0, y1);
        //mpz_mod(y, y, curve.q);
        mpz_invert(inverse, y1, curve.q);
        mpz_mul(y, y0, inverse);
        mpz_mod(y, y, curve.q);

        //big_int_destroy(u0);
        mpz_clear(u0);
        //big_int_destroy(u1);
        mpz_clear(u1);
        //big_int_destroy(u);
        mpz_clear(u);

        //big_int_destroy(v);
        mpz_clear(v);
        //big_int_destroy(v0);
        mpz_clear(v0);
        //big_int_destroy(v1);
        mpz_clear(v1);
        //big_int_destroy(v2);
        mpz_clear(v2);

        //big_int_destroy(CHIV);
        mpz_clear(CHIV);

        //big_int_destroy(X);
        mpz_clear(X);
        //big_int_destroy(X_plus_1);
        mpz_clear(X_plus_1);
        //big_int_destroy(X_plus_1_squared);
        mpz_clear(X_plus_1_squared);

        //big_int_destroy(Y0);
        mpz_clear(Y0);
        //big_int_destroy(Y1);
        mpz_clear(Y1);
        //big_int_destroy(Y2);
        mpz_clear(Y2);
        //big_int_destroy(Y3);
        mpz_clear(Y3);
        //big_int_destroy(C2);
        mpz_clear(C2);

        //big_int_destroy(x0);
        mpz_clear(x0);
        //big_int_destroy(rX);
        mpz_clear(rX);

        //big_int_destroy(y0);
        mpz_clear(y0);
        //big_int_destroy(y1);
        mpz_clear(y1);
    }

    //big_int_destroy(q_half);
    mpz_clear(q_half);

    // r->x = x;
    mpz_set(r->x, x);
    // r->y = y;
    mpz_set(r->y, y);

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
void gmp_elligator_1_point_to_string(mpz_t t, GMP_CurvePoint p, GMP_Curve curve)
{
    //BigInt *eta, *eta0, *eta1,
           //*eta_r, *q_exp,
           //*X, *X0,
           //*z, *z0, *z1, *z2, *z3,
           //*u,
           //*t0, *t1,
           //*q_half;
    mpz_t eta, eta0, eta1,
            eta_r, q_exp,
            X, X0,
            z, z0, z1, z2, z3,
            u,
            t0, t1,
            q_half, inverse;

    mpz_init(eta);
    mpz_init(eta0);
    mpz_init(eta1);
    mpz_init(eta_r);
    mpz_init(q_exp);
    mpz_init(X);
    mpz_init(X0);
    mpz_init(z);
    mpz_init(z0);
    mpz_init(z1);
    mpz_init(z2);
    mpz_init(z3);
    mpz_init(u);
    mpz_init(t0);
    mpz_init(t1);
    mpz_init(q_half);
    mpz_init(inverse);

    //eta0 = big_int_sub(NULL, p.y, big_int_one);
    mpz_sub_ui(eta0, p.y, 1); // 0x61c507d563235bea439c1d53bddf33e0d87c0041d3228cd0ca00c3d2a59025d

    //eta1 = big_int_add(NULL, p.y, big_int_one);
    mpz_add_ui(eta1, p.y, 1); // 0x61c507d563235bea439c1d53bddf33e0d87c0041d3228cd0ca00c3d2a59025f
    //eta1 = big_int_mul_mod(eta1, big_int_two, eta1, curve.q);
    mpz_mul_ui(eta1, eta1, 2);
    mpz_mod(eta1, eta1, curve.q); // 0x438a0faac646b7d487383aa77bbe67c1b0f80083a64519a1940187a54b204c7
    //eta = big_int_div_mod(eta0, eta0, eta1, curve.q);  // η = (y-1)/(2(y+1))
    //mpz_div(eta0, eta0, eta1);
    mpz_invert(inverse, eta1, curve.q); // 0xfdeaf0cc622f4ccb7e58f8b65ba2c8cd93fa3d716cfc203442904d8c74ae33
    mpz_mul(eta0, eta0, inverse); // 0x60f972cdd1e058a5053e34a2b41d9b0c7934583bf93dfd9c59f1611cfa712b332a00bee79db2acea4ee5b229ea58784525d50e59fabea9927e690dd7aae87
    mpz_mod(eta, eta0, curve.q); // 0x2042a1e673ba16669034e0e9348ba6e64d80b851d2607bf977adf64e716a396
    mpz_set(eta0, eta); // 0x2042a1e673ba16669034e0e9348ba6e64d80b851d2607bf977adf64e716a396

    //eta_r = big_int_mul_mod(NULL, eta, curve.r, curve.q);
    mpz_mul(eta_r, eta, curve.r); 
    mpz_mod(eta_r, eta_r, curve.q); // 0x65c0696e9286c9ad802ed257bd2290c769ade2be373d22dd6ab4db2c27ae52
    //eta_r = big_int_add(eta_r, big_int_one, eta_r);
    mpz_add_ui(eta_r, eta_r, 1); // 0x65c0696e9286c9ad802ed257bd2290c769ade2be373d22dd6ab4db2c27ae53
    //q_exp = big_int_add(NULL, curve.q, big_int_one);
    mpz_add_ui(q_exp, curve.q, 1); // 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff8
    //q_exp = big_int_div(q_exp, q_exp, big_int_four);
    mpz_fdiv_q_ui(q_exp, q_exp, 4); // 0x1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe

    //X0 = big_int_pow(NULL, eta_r, big_int_two, curve.q);
    mpz_powm_ui(X0, eta_r, 2, curve.q); // 0x7c31bb9461892aa81d069bb04dbf0cc99b0dcf181363c8df4e0c3775ed44a75
    //X0 = big_int_sub(X0, X0, big_int_one);
    mpz_sub_ui(X0, X0, 1); // 0x7c31bb9461892aa81d069bb04dbf0cc99b0dcf181363c8df4e0c3775ed44a74
    //X0 = big_int_pow(X0, X0, q_exp, curve.q);
    mpz_powm(X0, X0, q_exp, curve.q); // 0xe7c5f9fd8513685b7d93265780e4a332738ffd4100dda9b29dee72a745abfd
    //X = big_int_sub_mod(X0, X0, eta_r, curve.q); // X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)
    mpz_sub(X0, X0, eta_r);
    mpz_mod(X, X0, curve.q); // 0x8205908ef28c9eadfd6453ffc3c2126b09e21a82c9a086d53339977b1dfdaa

    //z0 = big_int_sub(NULL, curve.c, big_int_one);
    mpz_sub_ui(z0, curve.c, 1); // 0x4d1a3398ed42ceeb451d20824ca9cb49b69ef546bd7e6546aef19af1f9e49e0
    //z0 = big_int_mul_mod(z0, z0, curve.s, curve.q);
    mpz_mul(z0, z0, curve.s);
    mpz_mod(z0, z0, curve.q); // 0x67897dc1c0ccc95f80ac25cfb7dba3f085e0a97c32385ba7dc4079961d335b1
    //z0 = big_int_mul_mod(z0, z0, X, curve.q);
    mpz_mul(z0, z0, X);
    mpz_mod(z0, z0, curve.q); // 0x7289c8e3c691b2cdeda79b22f626bc14be9185783eae03fc78150da1a82b385
    //z1 = big_int_add(NULL, big_int_one, X);
    mpz_add_ui(z1, X, 1); // 0x8205908ef28c9eadfd6453ffc3c2126b09e21a82c9a086d53339977b1dfdab
    //z0 = big_int_mul_mod(z0, z0, z1, curve.q);
    mpz_mul(z0, z0, z1);
    mpz_mod(z0, z0, curve.q); // 0x7db431c5e5c193393bdd96c365cbb33fcc58461ad09a455ea95c93ee8910db
    //z0 = big_int_mul_mod(z0, z0, p.x, curve.q);
    mpz_mul(z0, z0, p.x);
    mpz_mod(z0, z0, curve.q); // 0x66f54486f406cc911e388dbabb7db5567e7705897f35f26cbdc7995b02a5bbe
    //z2 = big_int_pow(NULL, X, big_int_two, curve.q);
    mpz_powm_ui(z2, X, 2, curve.q); // 0x461594094bf365f210b33449a23391b6b3e9a3dfcdf2c4630b828a4dcb5283c
    //z3 = big_int_pow(NULL, curve.c, big_int_two, curve.q);
    mpz_powm_ui(z3, curve.c, 2, curve.q); // 0x257d2aeaca86b7a445ef47057ad0c8cf7a16154ab8860fa4f5ba21af86a4b25
    //z3 = big_int_inv(z3, z3, curve.q);
    mpz_invert(z3, z3, curve.q); // 0x771f18aed833220b34b0fdadefe83b1c247bccd1d7983a9438b412d3c3700ba
    //z2 = big_int_add(z2, z2, z3);
    mpz_add(z2, z2, z3); // 0xbd34acb8242687fd456431f7921bccd2d86570b1a58afef744369d218ec28f6
    //z0 = big_int_mul_mod(z0, z0, z2, curve.q);
    mpz_mul(z0, z0, z2);
    mpz_mod(z0, z0, curve.q); // 0x55f6286c0e9d7eddef448397c92b715fe48af231dd21f62cbf14f11fd7d932
    //z0 = big_int_chi(z0, z0, curve.q);
    mpz_chi(z0, z0, curve.q); // 1
    //z = big_int_mod(z0, z0, curve.q);  // z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))
    mpz_mod(z, z0, curve.q); // 1

    //u = big_int_mul_mod(NULL, z, X, curve.q); // u = zX
    mpz_mul(u, z, X);
    mpz_mod(u, u, curve.q); // 0x8205908ef28c9eadfd6453ffc3c2126b09e21a82c9a086d53339977b1dfdaa

    //t0 = big_int_sub(NULL, big_int_one, u);
    mpz_ui_sub(t0, 1, u); // -0x8205908ef28c9eadfd6453ffc3c2126b09e21a82c9a086d53339977b1dfda9
    //t1 = big_int_add(NULL, big_int_one, u);
    mpz_add_ui(t1, u, 1); // 0x8205908ef28c9eadfd6453ffc3c2126b09e21a82c9a086d53339977b1dfdab
    //t = big_int_div_mod(t, t0, t1, curve.q); // t = (1 − u)/(1 + u)
    //mpz_fdiv_q(t, t0, t1);
    mpz_invert(inverse, t1, curve.q); // 0x55e6f7891a2b3c4ccc3b2a190d5e55e6f7ef8
    mpz_mul(t, t0, inverse);
    mpz_mod(t, t, curve.q); // 0xabcdef1234567899987654321abcabcdefdef

    //q_half = big_int_sub(NULL, curve.q, big_int_one);
    mpz_sub_ui(q_half, curve.q, 1); // 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff6
    //q_half = big_int_div(q_half, q_half, big_int_two); // q_half = (q - 1)/2
    mpz_fdiv_q_ui(q_half, q_half, 2); // 0x3fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffb

    //if (big_int_compare(t, q_half) == 1) {
    if (mpz_cmp(t, q_half) == 1) {
        //t = big_int_neg(t, t);
        mpz_neg(t, t);
        //t = big_int_mod(t, t, curve.q);
        mpz_mod(t, t, curve.q);
    }

    //big_int_destroy(eta0);
    mpz_clear(eta0);
    //big_int_destroy(eta1);
    mpz_clear(eta1);

    //big_int_destroy(eta_r);
    mpz_clear(eta_r);
    //big_int_destroy(q_exp);
    mpz_clear(q_exp);

    //big_int_destroy(X0);
    mpz_clear(X0);

    //big_int_destroy(z0);
    mpz_clear(z0);
    //big_int_destroy(z1);
    mpz_clear(z1);
    //big_int_destroy(z2);
    mpz_clear(z2);
    //big_int_destroy(z3);
    mpz_clear(z3);

    //big_int_destroy(u);
    mpz_clear(u);

    //big_int_destroy(t0);
    mpz_clear(t0);
    //big_int_destroy(t1);
    mpz_clear(t1);
    mpz_clear(inverse);

    //return &t;
}
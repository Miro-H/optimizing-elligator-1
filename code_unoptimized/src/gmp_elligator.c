/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://github.com/Miro-H/optimizing-elligator-1).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact: miro.haller@alumni.ethz.ch
 *
 * Short description of this file:
 * This file implements the Elligator 1 mapping for Curve 1174 and its inverse
 * using the GMP library.
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

    mpz_t e;
    mpz_t e_minus_1;

    mpz_init(e);
    mpz_init(e_minus_1);

    // Check assumption that q is odd
    if (mpz_divisible_ui_p(q, 2))
        FATAL("Chi function implementation only supports odd q (actually prime q)!");

    if (!r)
    {
        mpz_init(r);
    }

    if (mpz_cmp_ui(t, 0) == 0)
    {
        mpz_set_ui(r, 0);
        mpz_clear(e);
        mpz_clear(e_minus_1);
        return;
    }

    mpz_set(e, q);
    mpz_sub_ui(e_minus_1, e, 1);
    mpz_fdiv_q_2exp(e, e_minus_1, 1);

    mpz_powm(r, t, e, q);

    mpz_clear(e);
    mpz_clear(e_minus_1);

    if (!mpz_cmp_ui(r, 0) || !mpz_cmp_ui(r, 1))
    {
        return;
    }
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
    mpz_set_str(curve->q,
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7", 16);

    mpz_set_si(curve->d, -1174);
    mpz_mod(curve->d, curve->d, curve->q);

    mpz_set_str(curve->s,
        "3FE707F0D7004FD334EE813A5F1A74AB2449139C82C39D84A09AE74CC78C615", 16);

    // c = 2 / s^2
    mpz_mul(curve->c, curve->s, curve->s);
    mpz_mod(curve->c, curve->c, curve->q);
    mpz_invert(inverse, curve->c, curve->q);
    mpz_mul_ui(curve->c, inverse, 2);
    mpz_mod(curve->c, curve->c, curve->q);

    // r = c + 1/c
    mpz_invert(curve->r, curve->c, curve->q);
    mpz_add(curve->r, curve->c, curve->r);
    mpz_mod(curve->r, curve->r, curve->q);
}


/**
* \brief Free elliptic curve
*/
void gmp_free_curve(GMP_Curve *curve)
{
    mpz_clear(curve->q);
    mpz_clear(curve->d);
    mpz_clear(curve->s);
    mpz_clear(curve->c);
    mpz_clear(curve->r);
}


/**
* \brief Free elliptic curve point
*/
void gmp_free_curve_point(GMP_CurvePoint *point)
{
    mpz_clear(point->x);
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
    mpz_sub_ui(q_half, curve.q, 1);
    mpz_fdiv_q_ui(q_half, q_half, 2);

    if (mpz_cmp_si(t, 0) == -1 || mpz_cmp(t, q_half) == 1)
        FATAL("Invalid input value for Elligator 1: t must be in the range [0, (q-1)/2]\n");

    if (mpz_cmp_si(t, 1) == 0)
    {
        mpz_set_si(x, 0);
        mpz_set_si(y, 1);
    }
    else {
        mpz_ui_sub(u0, 1, t);
        mpz_add_ui(u1, t, 1);
        mpz_invert(inverse, u1, curve.q);
        mpz_mul(u, u0, inverse);
        mpz_mod(u, u, curve.q);

        mpz_powm_ui(v0, u, 5, curve.q);
        mpz_powm_ui(v1, curve.r, 2, curve.q);
        mpz_sub_ui(v1, v1, 2);
        mpz_powm_ui(v2, u, 3, curve.q);
        mpz_mul(v1, v1, v2);
        mpz_mod(v1, v1, curve.q);
        mpz_add(v0, v0, v1);
        mpz_add(v0, v0, u);
        mpz_mod(v, v0, curve.q);

        mpz_chi(CHIV, v, curve.q);

        mpz_mul(X, CHIV, u);
        mpz_mod(X, X, curve.q);

        mpz_mul(Y0, CHIV, v);
        mpz_mod(Y0, Y0, curve.q);
        mpz_add_ui(Y1, curve.q, 1);
        mpz_fdiv_q_ui(Y2, Y1, 4);
        mpz_powm(Y0, Y0, Y2, curve.q);
        mpz_mul(Y0, Y0, CHIV);
        mpz_mod(Y0, Y0, curve.q);

        mpz_powm_ui(C2, curve.c, 2, curve.q);
        mpz_invert(C2, C2, curve.q);

        mpz_powm_ui(Y3, u, 2, curve.q);
        mpz_add(Y3, Y3, C2);
        mpz_mod(Y3, Y3, curve.q);
        mpz_chi(Y3, Y3, curve.q);
        mpz_mul(Y, Y0, Y3);
        mpz_mod(Y, Y, curve.q); // Y = (χ(v)v)**((q + 1) / 4)χ(v)χ(u**2 + 1 / c**2)

        mpz_add_ui(X_plus_1, X, 1);
        mpz_powm_ui(X_plus_1_squared, X_plus_1, 2, curve.q);

        mpz_sub_ui(x0, curve.c, 1);
        mpz_mul(x0, x0, curve.s);
        mpz_mod(x0, x0, curve.q);
        mpz_mul(x0, x0, X);
        mpz_mod(x0, x0, curve.q);
        mpz_mul(x0, x0, X_plus_1);
        mpz_mod(x0, x0, curve.q);
        mpz_invert(inverse, Y, curve.q);
        mpz_mul(x, x0, inverse);
        mpz_mod(x, x, curve.q); // x = (c − 1)*s*X*(1 + X) / Y

        mpz_mul(rX, curve.r, X);
        mpz_mod(rX, rX, curve.q);
        mpz_sub(y0, rX, X_plus_1_squared);

        mpz_add(y1, rX, X_plus_1_squared);
        mpz_invert(inverse, y1, curve.q);
        mpz_mul(y, y0, inverse);
        mpz_mod(y, y, curve.q); //  y = (rX − (1 + X)**2) / (rX + (1 + X)**2)

        mpz_clear(u0);
        mpz_clear(u1);
        mpz_clear(u);

        mpz_clear(v);
        mpz_clear(v0);
        mpz_clear(v1);
        mpz_clear(v2);

        mpz_clear(CHIV);

        mpz_clear(X);
        mpz_clear(X_plus_1);
        mpz_clear(X_plus_1_squared);

        mpz_clear(Y0);
        mpz_clear(Y1);
        mpz_clear(Y2);
        mpz_clear(Y3);
        mpz_clear(C2);

        mpz_clear(x0);
        mpz_clear(rX);

        mpz_clear(y0);
        mpz_clear(y1);
    }

    mpz_clear(q_half);

    mpz_set(r->x, x);
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

    mpz_sub_ui(eta0, p.y, 1);

    mpz_add_ui(eta1, p.y, 1);
    mpz_mul_ui(eta1, eta1, 2);
    mpz_mod(eta1, eta1, curve.q);
    mpz_invert(inverse, eta1, curve.q);
    mpz_mul(eta0, eta0, inverse);
    mpz_mod(eta, eta0, curve.q);
    mpz_set(eta0, eta); // η = (y-1)/(2(y+1))

    mpz_mul(eta_r, eta, curve.r);
    mpz_mod(eta_r, eta_r, curve.q);
    mpz_add_ui(eta_r, eta_r, 1);
    mpz_add_ui(q_exp, curve.q, 1);
    mpz_fdiv_q_ui(q_exp, q_exp, 4);

    mpz_powm_ui(X0, eta_r, 2, curve.q);
    mpz_sub_ui(X0, X0, 1);
    mpz_powm(X0, X0, q_exp, curve.q);
    mpz_sub(X0, X0, eta_r);
    mpz_mod(X, X0, curve.q); // X = −(1 + ηr) + ((1 + ηr)**2 − 1)**((q+1)/4)

    mpz_sub_ui(z0, curve.c, 1);
    mpz_mul(z0, z0, curve.s);
    mpz_mod(z0, z0, curve.q);
    mpz_mul(z0, z0, X);
    mpz_mod(z0, z0, curve.q);
    mpz_add_ui(z1, X, 1);
    mpz_mul(z0, z0, z1);
    mpz_mod(z0, z0, curve.q);
    mpz_mul(z0, z0, p.x);
    mpz_mod(z0, z0, curve.q);
    mpz_powm_ui(z2, X, 2, curve.q);
    mpz_powm_ui(z3, curve.c, 2, curve.q);
    mpz_invert(z3, z3, curve.q);
    mpz_add(z2, z2, z3);
    mpz_mul(z0, z0, z2);
    mpz_mod(z0, z0, curve.q);
    mpz_chi(z0, z0, curve.q);
    mpz_mod(z, z0, curve.q); // z = χ((c − 1)sX(1 + X)x(X**2 + 1/c**2))

    mpz_mul(u, z, X);
    mpz_mod(u, u, curve.q); // u = zX

    mpz_ui_sub(t0, 1, u);
    mpz_add_ui(t1, u, 1);
    mpz_invert(inverse, t1, curve.q);
    mpz_mul(t, t0, inverse);
    mpz_mod(t, t, curve.q); // t = (1 − u)/(1 + u)

    mpz_sub_ui(q_half, curve.q, 1);
    //q_half = big_int_div(q_half, q_half, big_int_two);
    mpz_fdiv_q_ui(q_half, q_half, 2); // q_half = (q - 1)/2

    if (mpz_cmp(t, q_half) == 1) {
        mpz_neg(t, t);
        mpz_mod(t, t, curve.q);
    }

    mpz_clear(eta0);
    mpz_clear(eta1);

    mpz_clear(eta_r);
    mpz_clear(q_exp);

    mpz_clear(X0);

    mpz_clear(z0);
    mpz_clear(z1);
    mpz_clear(z2);
    mpz_clear(z3);

    mpz_clear(u);

    mpz_clear(t0);
    mpz_clear(t1);
    mpz_clear(inverse);
}

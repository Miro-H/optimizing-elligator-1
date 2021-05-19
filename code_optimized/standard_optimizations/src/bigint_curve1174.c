/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements the BigInt operations specifically for the modulus
 * q = 2^251 - 9 from Curve1174.
 */

/*
 * Includes
 */
#include <stdlib.h>

// header files
#include "bigint_curve1174.h"
#include "debug.h"

/**
 * \brief Calculate r := a mod q
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_curve1174_mod(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_MOD);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(tmp);
    // big_int_div_rem(tmp, r, a, q);

    return r;
}


/**
 * \brief Calculate r := (a + b) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_add_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_ADD_MOD);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(r_loc);
    //
    // big_int_add(r_loc, a, b);
    // big_int_mod(r, r_loc, q);
    //
    // return r;
}


/**
 * \brief Calculate r := (a - b) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_sub_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_SUB_MOD);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(r_loc);
    //
    // big_int_sub(r_loc, a, b);
    // big_int_mod(r, r_loc, q);

    return r;
}


/**
 * \brief Calculate r := (a * b) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_mul_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_MUL_MOD);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(r_loc);
    //
    // big_int_mul(r_loc, a, b);
    // big_int_mod(r, r_loc, q);

    return r;
}


/**
 * \brief Calculate r := (a * b^-1) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_div_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_DIV_MOD);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(r_loc);
    //
    // // Write to local copy to be save against pointer reuse
    // big_int_inv(r_loc, b, q);
    // big_int_mul_mod(r, a, r_loc, q);

    return r;
}


/**
 * \brief Calculate r := a^-1 mod q (the inverse of a)
 *
 * \assumption r, a != NULL
 * \assumption a is invertible mod q, i.e., res.g = 1
 */
BigInt *big_int_curve1174_inv(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_INV);
    FATAL("Not yet optimized!");

    // TODO
    // EgcdResult res;
    //
    // big_int_egcd(&res, a, q);
    //
    // // set r = x mod q
    // big_int_mod(r, &res.x, q);

    return r;
}


/**
 * \brief Calculate r := (b^e) mod q
 *
 * \assumption r, b, e != NULL
 */
BigInt *big_int_curve1174_pow(BigInt *r, BigInt *b, BigInt *e)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_POW);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(e_loc);
    // BIG_INT_DEFINE_PTR(b_loc);
    //
    // BIG_INT_DEFINE_FROM_CHUNK(r_loc, 0, 1);
    //
    // big_int_copy(e_loc, e);
    // big_int_copy(b_loc, b);
    //
    // while (big_int_compare(e_loc, big_int_zero) > 0) {
    //     // If power is odd
    //     if (big_int_is_odd(e_loc))
    //         big_int_mul_mod(r_loc, r_loc, b_loc, q);
    //
    //     big_int_srl_small(e_loc, e_loc, 1);
    //     big_int_mul_mod(b_loc, b_loc, b_loc, q);
    // }
    //
    // // TODO: copy could be saved if we assume no aliasing
    // return big_int_copy(r, r_loc);
    return NULL;
}


/**
 * \brief Calculate the greatest common divisor using the extended Euclidean
 *        algorithm (iterative version) for the FIXED second argument q.
 *        This is mainly used for inverting numbers in Z_q.
 *
 * \returns Pointer to EgcdResult (x, y, g), where x * a + y * q = g
 *          and g is the GCD.
 *
 * \assumption r, a != NULL
 */
EgcdResult *big_int_curve1174_egcd(EgcdResult *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_EGCD);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(q);
    // BIG_INT_DEFINE_PTR(a_loc);
    // BIG_INT_DEFINE_PTR(x1);
    // BIG_INT_DEFINE_PTR(y1);
    // BIG_INT_DEFINE_PTR(tmp);
    //
    // // Avoid copying intermediate results to final result by using the latter
    // // from the start
    // BigInt *b_loc = &(r->g);
    // BigInt *x0 = &(r->x);
    // BigInt *y0 = &(r->y);
    //
    // if (big_int_is_zero(a) && big_int_is_zero(b)) {
    //     big_int_create_from_chunk(x0, 0, 0);
    //     big_int_create_from_chunk(y0, 0, 0);
    //     big_int_create_from_chunk(b_loc, 0, 0);
    //     return r;
    // }
    //
    // big_int_create_from_chunk(x0, 0, 0);
    // big_int_create_from_chunk(x1, 1, 0);
    // big_int_create_from_chunk(y0, 1, 0);
    // big_int_create_from_chunk(y1, 0, 0);
    //
    // big_int_copy(a_loc, a);
    // big_int_copy(b_loc, b);
    //
    // // Take the GCD of positive numbers, then just flig the signs of x, y at the end
    // big_int_abs(a_loc, a_loc);
    // big_int_abs(b_loc, b_loc);
    //
    // while (big_int_compare(a_loc, big_int_zero) != 0)
    // {
    //     big_int_div_rem(q, tmp, b_loc, a_loc);
    //     big_int_copy(b_loc, a_loc);
    //     big_int_copy(a_loc, tmp);
    //
    //     big_int_copy(tmp, y1);
    //     big_int_sub(y1, y0, big_int_mul(y1, y1, q));
    //     big_int_copy(y0, tmp);
    //
    //     big_int_copy(tmp, x1);
    //     big_int_sub(x1, x0, big_int_mul(x1, x1, q));
    //     big_int_copy(x0, tmp);
    // }
    //
    // // Account for signs of a and b
    // x0->sign ^= a->sign;
    // y0->sign ^= b->sign;

    return r;
}


/**
 * \brief Calculate the Chi function chi(t) = t**((q-1)/2) mod q
 * \returns 0 if t = 0, 1 if t is a non-zero square, -1 otherwise
 *
 * \assumption r, t != NULL
 */
BigInt *big_int_curve1174_chi(BigInt *r, BigInt *t)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_CHI);
    FATAL("Not yet optimized!");

    // TODO
    // BIG_INT_DEFINE_PTR(e);
    //
    // if (big_int_compare(t, big_int_zero) == 0) {
    //     // TODO: [Optimization] change return value to int8_t
    //     big_int_create_from_chunk(r, 0, 0);
    //     return r;
    // }
    //
    // big_int_copy(e, q);
    // big_int_srl_small(e, big_int_sub(e, e, big_int_one), 1);
    //
    // big_int_pow(r, t, e, q);
    //
    // if (!big_int_compare(r, big_int_zero) || !big_int_compare(r, big_int_one))
    //     return r;
    //
    // // TODO: [Optimization] change return value to int8_t
    // big_int_create_from_chunk(r, 1, 1); // r = -1
    return r;

}

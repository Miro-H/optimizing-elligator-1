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
 * \assumption a->size <= 16 (at most 512 bits)
 */
BigInt *big_int_curve1174_mod(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_MOD);

    BIG_INT_DEFINE_PTR(div_res);
    BIG_INT_DEFINE_PTR(a_upper);
    BigInt *a_lower;
    uint8_t a_sign;

    // TODO: Copy could be avoided if inplace modifications were allowed
    if (r != a)
        big_int_copy(r, a);

    // Preserve sign in case it is overwritten
    a_sign = a->sign;
    // Operate on absolute value afterwards
    r->sign = 0;

    // If a > 256 bits, use the special curve prime property:
    // 2^251 - 9 = 0 (mod q) => 2^251 = 9 (mod q) => 2^256 = 288 (mod q)
    // Thus, we can simplify:
    // a1 * 2^256 + a0 = a1 * 288 + a0 (mod q)
    if (a->size > Q_CHUNKS) {
        // TODO: maybe we could further optimize this multiplication by directly
        // doing it here or in general create an optimized "multiply with
        // single chunk" function
        big_int_srl_small(a_upper, r, 256);
        // Intentionally no mod reduction, since we do one later
        big_int_mul(a_upper, a_upper, big_int_288); // a1 * 288

        a_lower = r;
        a_lower->size = 8;
        big_int_curve1174_add_mod(r, a_lower, a_upper); // r = a1 * 288 + a0 (mod q)
    }
    // Case: q <= |a| < 2^256
    else if (big_int_curve1174_compare_to_q(r) != -1) {
        // TODO: could do subtractions instead of divrem. Since 2^256 / q < 33,
        // we have to do between `1` and `33` subtractions. Depends on optimized
        // subtraction whether this is beneficial. Or precompute a*q values
        // and do binary search.
        big_int_div_rem(div_res, r, r, q);
    }
    // else: case |a| < q: do nothing!

    // if a < 0, then a % q = q - (|a| % q)
    if (a_sign)
        big_int_sub(r, q, r);

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

    big_int_add(r, a, b);
    big_int_curve1174_mod(r, r);
    return r;
}


/**
 * \brief Calculate r := (a - b) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_sub_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_SUB_MOD);

    big_int_sub(r, a, b);
    big_int_curve1174_mod(r, r);
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

    // a, b are usually not larger than q, thus it is not worth it to perform a
    // mod operation on the operands before multiplying.
    // TODO: verify that even a check whether the operands are larger than q is
    // not worth it.

    big_int_mul(r, a, b);
    big_int_curve1174_mod(r, r);
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

    BIG_INT_DEFINE_PTR(r_loc);

    // Write to local copy to be save against pointer reuse
    BIG_INT_CURVE1174_INV(r_loc, b);
    big_int_curve1174_mul_mod(r, a, r_loc);
    return r;
}


/**
 * \brief Calculate r := a^-1 mod q (the inverse of a)
 *
 * Compute a^-1 = a^(q-2) (mod q) (a consequence of Fermat's theorem).
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_curve1174_inv_fermat(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1774_TYPE_BIG_INT_INV);

    big_int_curve1174_pow(r, a, q_min_two);
    return r;
}


// TODO: implement Montgommery inverse, which is based on cheaper shifting operations
// and compare it to fermat.


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

/**
 * \brief Compare a to q.
 *
 * \returns 1 if a > q, -1 if a < q, otherwise 0
 *
 * \assumption a != NULL
 */
int8_t big_int_curve1174_compare_to_q(BigInt *a)
{
    // Easy cases where a is clearly larger/smaller
    if (a->size > Q_CHUNKS)
        return 1;
    if (a->size < Q_CHUNKS || a->sign)
        return -1;

    // Remaining case: a->size == Q_CHUNKS and a is positive

    // Compare to highest chunk of Q only
    if (a->chunks[Q_CHUNKS-1] < Q_MSB_CHUNK)
        return -1;
    if (a->chunks[Q_CHUNKS-1] > Q_MSB_CHUNK)
        return 1;
    return 0;
}

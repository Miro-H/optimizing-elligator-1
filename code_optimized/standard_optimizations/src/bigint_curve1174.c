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
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_ADD_MOD);

    BIG_INT_DEFINE_PTR(a_mod);
    BIG_INT_DEFINE_PTR(b_mod);
    BigInt *a_loc, *b_loc;

    if (a->sign || big_int_curve1174_compare_to_q(r) >= 0) {
        big_int_curve1174_mod(a_mod, a);
        a_loc = a_mod;
    }
    else {
        a_loc = a;
    }

    if (b->sign || big_int_curve1174_compare_to_q(r) >= 0) {
        big_int_curve1174_mod(b_mod, b);
        b_loc = b_mod;
    }
    else {
        b_loc = b;
    }

    big_int_add(r, a_loc, b_loc);

    if (r->sign)
        big_int_add(r, r, q);
    else if (big_int_curve1174_compare_to_q(r) >= 0)
        big_int_sub(r, r, q);

    return r;
}


/**
 * \brief Calculate r := (a - b) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_sub_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_SUB_MOD);

    big_int_sub(r, a, b);

    // if (big_int_curve1174_compare_to_q(r) >= 0)
    //     big_int_sub(r, r, q);
    // else if (r->sign)
    //     big_int_add(r, r, q);
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
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_MUL_MOD);

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
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_DIV_MOD);

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
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_INV);

    big_int_curve1174_pow(r, a, q_min_two);
    return r;
}


// TODO: implement Montgommery inverse, which is based on cheaper shifting operations
// and compare it to fermat.


/**
 * \brief Calculate r := (b^e) mod q for small exponents e < 2^64
 *
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_small(BigInt *r, BigInt *b, uint64_t e)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW_SMALL);

    BIG_INT_DEFINE_PTR(b_loc);
    BIG_INT_DEFINE_FROM_CHUNK(r_one, 0, 1);
    BigInt *r_loc;

    if (r == b)
        r_loc = r_one;
    else
        r_loc = big_int_create_from_chunk(r, 1, 0);

    big_int_copy(b_loc, b);

    while (e) {
        // If power is odd
        if (e & 1)
            big_int_curve1174_mul_mod(r_loc, r_loc, b_loc);

        e >>= 1;
        // TODO: compute those in parallel in first step. Those are only 256
        // results, we could even store them on the stack.
        big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
    }

    if (r == b)
        big_int_copy(r, r_loc);

    return r;
}

/**
 * \brief Calculate r := (b^e) mod q
 *
 * \assumption r, b, e != NULL
 */
BigInt *big_int_curve1174_pow(BigInt *r, BigInt *b, BigInt *e)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW);

    BIG_INT_DEFINE_PTR(b_loc);
    BIG_INT_DEFINE_FROM_CHUNK(r_one, 0, 1);
    BigInt *r_loc;
    dbl_chunk_size_t e_chunk;

    if (r == b)
        r_loc = r_one;
    else
        r_loc = big_int_create_from_chunk(r, 1, 0);

    big_int_copy(b_loc, b);

    // Operate on exponent chunk by chunk
    for (uint32_t i = 0; i < e->size; ++i) {
        e_chunk = e->chunks[i];

        while (e_chunk) {
            // If power is odd
            if (e_chunk & 1)
                big_int_curve1174_mul_mod(r_loc, r_loc, b_loc);

            e_chunk >>= 1;
            // TODO: compute those in parallel in first step. Those are only 256
            // results, we could even store them on the stack.
            big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
        }
    }

    if (r == b)
        big_int_copy(r, r_loc);

    return r;
}


/**
 * \brief Calculate r := (b^((q-1)/2)) mod q
 *
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_q_m1_d2(BigInt *r, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW_1_2);

    BIG_INT_DEFINE_PTR(b_loc);
    BIG_INT_DEFINE_FROM_CHUNK(r_one, 0, 1);
    BigInt *r_loc;

    if (r == b)
        r_loc = r_one;
    else
        r_loc = big_int_create_from_chunk(r, 1, 0);

    big_int_copy(b_loc, b);

    // (q-1)/2 = 0b1111...11111011 (there are 247 ones before the suffix 011)

    // Ensure suffix
    // e = 1
    big_int_curve1174_mul_mod(r_loc, r_loc, b_loc);

    // e = 11
    big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
    big_int_curve1174_mul_mod(r_loc, r_loc, b_loc);

    // e = 011
    big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);

    // All the remaining bits are set to one, so we add all of them
    for (uint32_t i = 0; i < 247; ++i) {
        big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
        big_int_curve1174_mul_mod(r_loc, r_loc, b_loc);
    }

    if (r == b)
        big_int_copy(r, r_loc);

    return r;
}



/**
 * \brief Calculate r := (b^((q+1)/4)) mod q
 *
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_q_p1_d4(BigInt *r, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW_1_2);

    BIG_INT_DEFINE_PTR(b_loc);
    BIG_INT_DEFINE_FROM_CHUNK(r_one, 0, 1);
    BigInt *r_loc;

    if (r == b)
        r_loc = r_one;
    else
        r_loc = big_int_create_from_chunk(r, 1, 0);

    big_int_copy(b_loc, b);

    // (q+1)/4 = 0b111111...111110 (there are 248 ones)

    // The first bit of the exponent is zero, because we start with doubling b.
    // All the remaining bits are set to one, so we add all of them.
    for (uint32_t i = 0; i < 248; ++i) {
        big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
        big_int_curve1174_mul_mod(r_loc, r_loc, b_loc);
    }

    if (r == b)
        big_int_copy(r, r_loc);

    return r;
}


/**
 * \brief Calculate the Chi function chi(t) = t**((q-1)/2) mod q
 *
 * \returns 0 if t is a non-zero square, 1 otherwise
 *          NOTE: this deviates from the normal definition of Chi, but is better
 *          for our internal BigInt representation.
 *
 * \assumption r, t != NULL
 * \assumption t =/= 0 (guaranteed by proof in section 3.2 of the Elligator paper)
 *             as a consequence of XY =/= 0.
 */
int8_t big_int_curve1174_chi(BigInt *t)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_CHI);

    BIG_INT_DEFINE_PTR(r_loc);
    big_int_curve1174_pow_q_m1_d2(r_loc, t);

    // assumption: r = 1 or -1 after squaring (i.e., always a single chunk)
    // Note that -1 mod q is larger than one chunk.
    return r_loc->size != 1;
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

    // Compare to highest chunk of Q
    if (a->chunks[Q_CHUNKS-1] < Q_MSB_CHUNK)
        return -1;
    if (a->chunks[Q_CHUNKS-1] > Q_MSB_CHUNK)
        return 1;
    // Compare to lowest chunk of Q
    if (a->chunks[0] < Q_LSB_CHUNK)
        return -1;
    // If the lowest chunk is larger, compare to intermediate chunks
    if (a->chunks[0] > Q_LSB_CHUNK) {
        for (uint32_t i = 1; i < Q_CHUNKS-1; ++i) {
            if (a->chunks[i] < Q_INTERMEDIATE_CHUNK)
                return -1;
        }
        return 1;
    }
    return 0;
}

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
#include "optimization_flags.h"
#include "bigint_curve1174.h"
#include "debug.h"

// True if r is smaller or equal than the compared a_i * q
#define MOD_BIGINT_COMP_LEQ(r, chunk_7, chunk_0)                    \
    (r)->chunks[7] < (chunk_7)                                      \
    || ((r)->chunks[7] == (chunk_7) && r->chunks[0] <= (chunk_0))

/**
 * \brief Calculate r := a mod q
 *        NOTE: this performs inplace modifications!
 *
 * \assumption r != NULL
 * \assumption a->size <= 16 (at most 512 bits)
 */
BigInt *big_int_curve1174_mod(BigInt *r)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_MOD);

    BIG_INT_DEFINE_PTR(r_upper);
    BigInt *r_lower;
    uint8_t orig_sign;

    // Preserve sign in case it is overwritten
    orig_sign = r->sign;
    // Operate on absolute value afterwards
    r->sign = 0;

    // If a > 256 bits, use the special curve prime property:
    // 2^251 - 9 = 0 (mod q) => 2^251 = 9 (mod q) => 2^256 = 288 (mod q)
    // Thus, we can simplify:
    // a1 * 2^256 + a0 = a1 * 288 + a0 (mod q)
    if (r->size > Q_CHUNKS) {
        // TODO: maybe we could further optimize this multiplication by directly
        // doing it here or in general create an optimized "multiply with
        // single chunk" function
        big_int_srl_small(r_upper, r, 256);

        // Intentionally no mod reduction, since we do one later. We know that
        // our intermediate values are never larger than (q-1)^2 and 288 * (q-1)^2 < 2^512
        big_int_mul(r_upper, r_upper, big_int_288); // a1 * 288

        r_lower = r;
        r_lower->size = 8;
        big_int_curve1174_add_mod(r, r_lower, r_upper); // r = a1 * 288 + a0 (mod q)
    }
    // Case: q <= |a| < 2^256
    else if (big_int_curve1174_compare_to_q(r) != -1) {
        // Note that all multiples of q have chunks 1 to 6 as 0xffffffff.
        // Also note that in all except negligibly few cases, r has 8 chunks, so,
        // we optimize for that case.
        // We can only compare chunk 7 and chunk 0: if they are equal
        // to those of the number, the number is smaller or equal. We build our comparison
        // such that those cases fall in the same branch.

        if (MOD_BIGINT_COMP_LEQ(r, Q_17_CHUNK_7, Q_17_CHUNK_0)) {
            if (MOD_BIGINT_COMP_LEQ(r, Q_9_CHUNK_7, Q_9_CHUNK_0)) {
                if (MOD_BIGINT_COMP_LEQ(r, Q_5_CHUNK_7, Q_5_CHUNK_0)) {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_3_CHUNK_7, Q_3_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_2)) {
                            big_int_sub(r, r, q);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_3))) {
                                big_int_sub(r, r, q_2);
                            }
                            else {
                                big_int_sub(r, r, q_3);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_4)) {
                            big_int_sub(r, r, q_3);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_5))) {
                                big_int_sub(r, r, q_4);
                            }
                            else {
                                big_int_sub(r, r, q_5);
                            }
                        }
                    }
                }
                else {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_7_CHUNK_7, Q_7_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_6)) {
                            big_int_sub(r, r, q_5);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_7))) {
                                big_int_sub(r, r, q_6);
                            }
                            else {
                                big_int_sub(r, r, q_7);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_8)) {
                            big_int_sub(r, r, q_7);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_9))) {
                                big_int_sub(r, r, q_8);
                            }
                            else {
                                big_int_sub(r, r, q_9);
                            }
                        }
                    }
                }
            }
            else {
                if (MOD_BIGINT_COMP_LEQ(r, Q_13_CHUNK_7, Q_13_CHUNK_0)) {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_11_CHUNK_7, Q_11_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_10)) {
                            big_int_sub(r, r, q_9);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_11))) {
                                big_int_sub(r, r, q_10);
                            }
                            else {
                                big_int_sub(r, r, q_11);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_12)) {
                            big_int_sub(r, r, q_11);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_13))) {
                                big_int_sub(r, r, q_12);
                            }
                            else {
                                big_int_sub(r, r, q_13);
                            }
                        }
                    }
                }
                else {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_15_CHUNK_7, Q_15_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_14)) {
                            big_int_sub(r, r, q_13);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_15))) {
                                big_int_sub(r, r, q_14);
                            }
                            else {
                                big_int_sub(r, r, q_15);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_16)) {
                            big_int_sub(r, r, q_15);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_17))) {
                                big_int_sub(r, r, q_16);
                            }
                            else {
                                big_int_sub(r, r, q_17);
                            }
                        }
                    }
                }
            }
        }
        else {
            if (MOD_BIGINT_COMP_LEQ(r, Q_25_CHUNK_7, Q_25_CHUNK_0)) {
                if (MOD_BIGINT_COMP_LEQ(r, Q_21_CHUNK_7, Q_21_CHUNK_0)) {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_19_CHUNK_7, Q_19_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_18)) {
                            big_int_sub(r, r, q_17);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_19))) {
                                big_int_sub(r, r, q_18);
                            }
                            else {
                                big_int_sub(r, r, q_19);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_20)) {
                            big_int_sub(r, r, q_19);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_21))) {
                                big_int_sub(r, r, q_20);
                            }
                            else {
                                big_int_sub(r, r, q_21);
                            }
                        }
                    }
                }
                else {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_23_CHUNK_7, Q_23_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_22)) {
                            big_int_sub(r, r, q_21);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_23))) {
                                big_int_sub(r, r, q_22);
                            }
                            else {
                                big_int_sub(r, r, q_23);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_24)) {
                            big_int_sub(r, r, q_23);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_25))) {
                                big_int_sub(r, r, q_24);
                            }
                            else {
                                big_int_sub(r, r, q_25);
                            }
                        }
                    }
                }
            }
            else {
                if (MOD_BIGINT_COMP_LEQ(r, Q_29_CHUNK_7, Q_29_CHUNK_0)) {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_27_CHUNK_7, Q_27_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_26)) {
                            big_int_sub(r, r, q_25);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_27))) {
                                big_int_sub(r, r, q_26);
                            }
                            else {
                                big_int_sub(r, r, q_27);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_28)) {
                            big_int_sub(r, r, q_27);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_29))) {
                                big_int_sub(r, r, q_28);
                            }
                            else {
                                big_int_sub(r, r, q_29);
                            }
                        }
                    }
                }
                else {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_31_CHUNK_7, Q_31_CHUNK_0)) {
                        if (big_int_curve1174_lt_aq(r, q_30)) {
                            big_int_sub(r, r, q_29);
                        }
                        else {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_31))) {
                                big_int_sub(r, r, q_30);
                            }
                            else {
                                big_int_sub(r, r, q_31);
                            }
                        }
                    }
                    else {
                        if (big_int_curve1174_lt_aq(r, q_32)) {
                            big_int_sub(r, r, q_31);
                        }
                        else {
                            if (big_int_curve1174_lt_aq(r, q_33)) {
                                big_int_sub(r, r, q_32);
                            }
                            else {
                                big_int_sub(r, r, q_33);
                            }
                        }
                    }
                }
            }
        }
    }
    // else: case |a| < q: do nothing!

    // if a < 0, then a % q = q - (|a| % q)
    if (orig_sign)
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

    big_int_add(r, a, b);
    big_int_curve1174_mod(r);

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
    return big_int_curve1174_mod(r);
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
    return big_int_curve1174_mod(r);
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

    big_int_curve1174_pow(r, a, q_m2);
    return r;
}


// TODO: implement Montgommery inverse, which is based on cheaper shifting operations
// and compare it to fermat.


/**
 * \brief Calculate r := (b^e) mod q for small exponents e < 2^64
 *
 * \assumption r != b, i.e., NO ALIASING
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_small(BigInt *r, BigInt *b, uint64_t e)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW_SMALL);

    BIG_INT_DEFINE_PTR(b_loc);

    big_int_create_from_chunk(r, 1, 0);
    big_int_copy(b_loc, b);

    while (e) {
        // If power is odd
        if (e & 1)
            big_int_curve1174_mul_mod(r, r, b_loc);

        e >>= 1;
        // TODO: compute those in parallel in first step. Those are only 256
        // results, we could even store them on the stack.
        big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
    }

    return r;
}

/**
 * \brief Calculate r := (b^e) mod q
 *
 * \assumption r != b, i.e., NO ALIASING
 * \assumption r, b, e != NULL
 */
BigInt *big_int_curve1174_pow(BigInt *r, BigInt *b, BigInt *e)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW);

    BIG_INT_DEFINE_PTR(b_loc);
    dbl_chunk_size_t e_chunk;

    big_int_create_from_chunk(r, 1, 0);
    big_int_copy(b_loc, b);

    // Operate on exponent chunk by chunk
    for (uint32_t i = 0; i < e->size; ++i) {
        e_chunk = e->chunks[i];

        while (e_chunk) {
            // If power is odd
            if (e_chunk & 1)
                big_int_curve1174_mul_mod(r, r, b_loc);

            e_chunk >>= 1;
            big_int_curve1174_mul_mod(b_loc, b_loc, b_loc);
        }
    }

    return r;
}


/**
 * \brief Calculate r := (b^((q-1)/2)) mod q
 *
 * \assumption r = 1, i.e., it is already initialized by the caller
 * \assumption r != b, i.e., NO ALIASING
 * \assumption b is MODIFIED inplace. The caller MUST NOT rely on its value.
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_q_m1_d2(BigInt *r, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_CURVE1174_TYPE_BIG_INT_POW_1_2);

    // (q-1)/2 = 0b1111...11111011 (there are 247 ones before the suffix 011)

    // Ensure suffix
    // e = 1
    big_int_curve1174_mul_mod(r, r, b);

    // e = 11
    big_int_curve1174_mul_mod(b, b, b);
    big_int_curve1174_mul_mod(r, r, b);

    // e = 011
    big_int_curve1174_mul_mod(b, b, b);

    // All the remaining bits are set to one, so we add all of them
    for (uint32_t i = 0; i < 247; ++i) {
        big_int_curve1174_mul_mod(b, b, b);
        big_int_curve1174_mul_mod(r, r, b);
    }

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

    BIG_INT_DEFINE_FROM_CHUNK(r_loc, 0, 1);

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

    // Compare to intermediate chunks
    for (uint32_t i = Q_CHUNKS - 2; i >= 1; --i) {
        if (a->chunks[i] < Q_INTERMEDIATE_CHUNK)
            return -1;
    }
    return (a->chunks[0] > Q_LSB_CHUNK) ? 1 : 0;
}

/**
 * \brief Compare b to a * q for a \in [2, 33]
 *
 * \returns 0 if b >= a * q, 1 if b < a * q
 *
 * \assumption b, aq != NULL
 * \assumption b >= 0
 */
int8_t big_int_curve1174_lt_aq(BigInt *b, BigInt *aq)
{
    // Compare to highest chunk of a_q
    if (b->size < aq->size || b->chunks[b->size - 1] < aq->chunks[aq->size - 1])
        return 1;
    if (b->size > aq->size || b->chunks[b->size - 1] > aq->chunks[aq->size - 1])
        return 0;

    // Compare to lowest chunk of Q
    if (b->chunks[0] < aq->chunks[0])
        return 1;

    // Compare to intermediate chunks
    for (uint32_t i = aq->size - 2; i >= 1; --i) {
        if (b->chunks[i] < Q_INTERMEDIATE_CHUNK)
            return 1;
    }
    return 0;
}

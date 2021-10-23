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
#define MOD_BIGINT_COMP_LEQ(r, chunk_7, chunk_0) \
    (r)->chunks[7] < (chunk_7) || ((r)->chunks[7] == (chunk_7) && r->chunks[0] <= (chunk_0))

/**
 * \brief Calculate r := a mod q
 *        NOTE: this performs inplace modifications!
 *
 * \assumption r != NULL
 * \assumption a->size <= 16 (at most 512 bits)
 */
BigInt *big_int_curve1174_mod(BigInt *r)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_MOD);

    BIG_INT_DEFINE_PTR(r_upper);
    BIG_INT_DEFINE_PTR(r_upper_288);
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
    if (r->size > Q_CHUNKS)
    {
        // Do right shift by 256 with a memcpy because this is a special case and
        // we can save a call to big_int_srl_small this way.
        r_upper->sign = r->sign;
        r_upper->size = r->size - Q_CHUNKS;
        memcpy((void *)r_upper->chunks,
               (void *)(r->chunks + Q_CHUNKS),
               r_upper->size * BIGINT_INTERNAL_CHUNK_BYTE);
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        // Intentionally no mod reduction, since we do one later. We know that
        // our intermediate values are never larger than (q-1)^2 and 288 * (q-1)^2 < 2^512
        big_int_mul(r_upper_288, r_upper, big_int_288); // a1 * 288

        r_lower = r;
        r_lower->size = 8;
        big_int_curve1174_add_mod(r, r_lower, r_upper_288); // r = a1 * 288 + a0 (mod q)
    }
    // Case: q <= |a| < 2^256
    else if (big_int_curve1174_compare_to_q(r) != -1)
    {
        // Note that all multiples of q have chunks 1 to 6 as 0xffffffff.
        // Also note that in all except negligibly few cases, r has 8 chunks, so,
        // we optimize for that case.
        // We can only compare chunk 7 and chunk 0: if they are equal
        // to those of the number, the number is smaller or equal. We build our comparison
        // such that those cases fall in the same branch.

        if (MOD_BIGINT_COMP_LEQ(r, Q_17_CHUNK_7, Q_17_CHUNK_0))
        {
            if (MOD_BIGINT_COMP_LEQ(r, Q_9_CHUNK_7, Q_9_CHUNK_0))
            {
                if (MOD_BIGINT_COMP_LEQ(r, Q_5_CHUNK_7, Q_5_CHUNK_0))
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_3_CHUNK_7, Q_3_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_2))
                        {
                            big_int_sub(r, r, q);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_3)))
                            {
                                big_int_sub(r, r, q_2);
                            }
                            else
                            {
                                big_int_sub(r, r, q_3);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_4))
                        {
                            big_int_sub(r, r, q_3);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_5)))
                            {
                                big_int_sub(r, r, q_4);
                            }
                            else
                            {
                                big_int_sub(r, r, q_5);
                            }
                        }
                    }
                }
                else
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_7_CHUNK_7, Q_7_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_6))
                        {
                            big_int_sub(r, r, q_5);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_7)))
                            {
                                big_int_sub(r, r, q_6);
                            }
                            else
                            {
                                big_int_sub(r, r, q_7);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_8))
                        {
                            big_int_sub(r, r, q_7);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_9)))
                            {
                                big_int_sub(r, r, q_8);
                            }
                            else
                            {
                                big_int_sub(r, r, q_9);
                            }
                        }
                    }
                }
            }
            else
            {
                if (MOD_BIGINT_COMP_LEQ(r, Q_13_CHUNK_7, Q_13_CHUNK_0))
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_11_CHUNK_7, Q_11_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_10))
                        {
                            big_int_sub(r, r, q_9);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_11)))
                            {
                                big_int_sub(r, r, q_10);
                            }
                            else
                            {
                                big_int_sub(r, r, q_11);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_12))
                        {
                            big_int_sub(r, r, q_11);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_13)))
                            {
                                big_int_sub(r, r, q_12);
                            }
                            else
                            {
                                big_int_sub(r, r, q_13);
                            }
                        }
                    }
                }
                else
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_15_CHUNK_7, Q_15_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_14))
                        {
                            big_int_sub(r, r, q_13);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_15)))
                            {
                                big_int_sub(r, r, q_14);
                            }
                            else
                            {
                                big_int_sub(r, r, q_15);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_16))
                        {
                            big_int_sub(r, r, q_15);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_17)))
                            {
                                big_int_sub(r, r, q_16);
                            }
                            else
                            {
                                big_int_sub(r, r, q_17);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (MOD_BIGINT_COMP_LEQ(r, Q_25_CHUNK_7, Q_25_CHUNK_0))
            {
                if (MOD_BIGINT_COMP_LEQ(r, Q_21_CHUNK_7, Q_21_CHUNK_0))
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_19_CHUNK_7, Q_19_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_18))
                        {
                            big_int_sub(r, r, q_17);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_19)))
                            {
                                big_int_sub(r, r, q_18);
                            }
                            else
                            {
                                big_int_sub(r, r, q_19);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_20))
                        {
                            big_int_sub(r, r, q_19);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_21)))
                            {
                                big_int_sub(r, r, q_20);
                            }
                            else
                            {
                                big_int_sub(r, r, q_21);
                            }
                        }
                    }
                }
                else
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_23_CHUNK_7, Q_23_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_22))
                        {
                            big_int_sub(r, r, q_21);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_23)))
                            {
                                big_int_sub(r, r, q_22);
                            }
                            else
                            {
                                big_int_sub(r, r, q_23);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_24))
                        {
                            big_int_sub(r, r, q_23);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_25)))
                            {
                                big_int_sub(r, r, q_24);
                            }
                            else
                            {
                                big_int_sub(r, r, q_25);
                            }
                        }
                    }
                }
            }
            else
            {
                if (MOD_BIGINT_COMP_LEQ(r, Q_29_CHUNK_7, Q_29_CHUNK_0))
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_27_CHUNK_7, Q_27_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_26))
                        {
                            big_int_sub(r, r, q_25);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_27)))
                            {
                                big_int_sub(r, r, q_26);
                            }
                            else
                            {
                                big_int_sub(r, r, q_27);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_28))
                        {
                            big_int_sub(r, r, q_27);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_29)))
                            {
                                big_int_sub(r, r, q_28);
                            }
                            else
                            {
                                big_int_sub(r, r, q_29);
                            }
                        }
                    }
                }
                else
                {
                    if (MOD_BIGINT_COMP_LEQ(r, Q_31_CHUNK_7, Q_31_CHUNK_0))
                    {
                        if (big_int_curve1174_lt_aq(r, q_30))
                        {
                            big_int_sub(r, r, q_29);
                        }
                        else
                        {
                            if (LIKELY(big_int_curve1174_lt_aq(r, q_31)))
                            {
                                big_int_sub(r, r, q_30);
                            }
                            else
                            {
                                big_int_sub(r, r, q_31);
                            }
                        }
                    }
                    else
                    {
                        if (big_int_curve1174_lt_aq(r, q_32))
                        {
                            big_int_sub(r, r, q_31);
                        }
                        else
                        {
                            if (big_int_curve1174_lt_aq(r, q_33))
                            {
                                big_int_sub(r, r, q_32);
                            }
                            else
                            {
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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_ADD_MOD);

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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_SUB_MOD);

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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_MUL_MOD);

    // a, b are usually not larger than q, thus it is not worth it to perform a
    // mod operation on the operands before multiplying.
    big_int_mul(r, a, b);
    return big_int_curve1174_mod(r);
}

/**
 * \brief Calculate r := a^2 mod q
 *
 * \assumption r != a, i.e., NO ALIASING
 * \assumption r, a != NULL
 */
BigInt *big_int_curve1174_square_mod(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_SQUARED_MOD);

    big_int_square(r, a);
    return big_int_curve1174_mod(r);
}

/**
 * \brief Calculate r := (a * b^-1) mod q
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_curve1174_div_mod(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_DIV_MOD);

    BIG_INT_DEFINE_PTR(r_loc);

    // Write to local copy to be save against pointer reuse
    big_int_curve1174_inv_fermat(r_loc, b);
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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_INV);

    big_int_curve1174_pow_q_m2(r, a);
    return r;
}

// TODO: consider implementing Montgommery inverse, which are based on cheaper
// shifting operations and compare it to fermat.

/**
 * \brief Calculate r := (b^e) mod q for small exponents e < 2^64
 *
 * \assumption r != b, i.e., NO ALIASING
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_small(BigInt *r, BigInt *b, uint64_t e)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_POW_SMALL);

    BIG_INT_DEFINE_PTR(b_loc1);
    BIG_INT_DEFINE_PTR(b_loc2);

    BIG_INT_DEFINE_FROM_CHUNK(r2, 0, 1);

    BigInt *r_tmp, *r1;

    r1 = r;

    big_int_copy(b_loc1, b);

    while (e)
    {
        // If power is odd
        if (e & 1)
        {
            big_int_curve1174_mul_mod(r1, r2, b_loc1);
            r_tmp = r1;
            r1 = r2;
            r2 = r_tmp;
        }
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        e >>= 1;
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        // Early exit: avoid cleanup due to unrolling, avoid unnecessary square op
        if (!e)
            break;

        big_int_curve1174_square_mod(b_loc2, b_loc1);

        // ------ Unroll ------
        if (e & 1)
        {
            big_int_curve1174_mul_mod(r1, r2, b_loc2);
            r_tmp = r1;
            r1 = r2;
            r2 = r_tmp;
        }
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        e >>= 1;
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        // Early exit: avoid unnecessary square op
        if (e)
            big_int_curve1174_square_mod(b_loc1, b_loc2);
    }

    if (r != r2)
        big_int_copy(r, r2);

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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_POW);

    BIG_INT_DEFINE_PTR(b_loc1);
    BIG_INT_DEFINE_PTR(b_loc2);

    dbl_chunk_size_t e_chunk;
    uint32_t i, j;

    BIG_INT_DEFINE_FROM_CHUNK(r2, 0, 1);

    BigInt *r_tmp, *r1;

    r1 = r;
    big_int_copy(b_loc1, b);

    // Operate on exponent chunk by chunk
    for (i = 0; i < e->size - 1; ++i)
    {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        e_chunk = e->chunks[i];

        for (j = 0; j < BIGINT_CHUNK_BIT_SIZE; ++j)
        {
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            // If power is odd
            if (e_chunk & 1)
            {
                big_int_curve1174_mul_mod(r1, r2, b_loc1);
                r_tmp = r1;
                r1 = r2;
                r2 = r_tmp;
            }
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            e_chunk >>= 1;
            ADD_STAT_COLLECTION(BASIC_SHIFT)
            big_int_curve1174_square_mod(b_loc2, b_loc1);

            // -- unroll --
            if (e_chunk & 1)
            {
                big_int_curve1174_mul_mod(r1, r2, b_loc2);
                r_tmp = r1;
                r1 = r2;
                r2 = r_tmp;
            }
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            e_chunk >>= 1;
            ADD_STAT_COLLECTION(BASIC_SHIFT)

            big_int_curve1174_square_mod(b_loc1, b_loc2);
        }
    }

    // Special case for the last chunk
    e_chunk = e->chunks[e->size - 1];
    for (j = 0; j < BIGINT_CHUNK_BIT_SIZE; ++j)
    {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        // If power is odd
        if (e_chunk & 1)
        {
            big_int_curve1174_mul_mod(r1, r2, b_loc1);
            r_tmp = r1;
            r1 = r2;
            r2 = r_tmp;
        }
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        e_chunk >>= 1;
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        // Early exit: avoid cleanup due to unrolling, avoid unnecessary square op
        if (!e_chunk)
            break;

        big_int_curve1174_square_mod(b_loc2, b_loc1);

        // -- unroll --
        if (e_chunk & 1)
        {
            big_int_curve1174_mul_mod(r1, r2, b_loc2);
            r_tmp = r1;
            r1 = r2;
            r2 = r_tmp;
        }
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        e_chunk >>= 1;
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        // Early exit: avoid unnecessary square op
        if (e)
            big_int_curve1174_square_mod(b_loc1, b_loc2);

        big_int_curve1174_square_mod(b_loc1, b_loc2);
    }

    if (r != r2)
        big_int_copy(r, r2);

    return r;
}

/**
 * \brief Calculate r := (b^((q-1)/2)) mod q
 *
 * \assumption r != b, i.e., NO ALIASING
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_q_m1_d2(BigInt *r, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_POW_Q_M1_D2);

    BIG_INT_DEFINE_PTR(b_loc_1);
    BIG_INT_DEFINE_PTR(b_loc_2);
    BIG_INT_DEFINE_PTR(r_loc);

    // (q-1)/2 = 0b1111...11111011 (there are 247 ones before the suffix 011)

    // Ensure suffix

    // e = 11
    big_int_curve1174_square_mod(b_loc_1, b);
    big_int_curve1174_mul_mod(r_loc, b, b_loc_1);

    // e = 011
    big_int_curve1174_square_mod(b_loc_2, b_loc_1);

    // All the remaining bits are set to one, so we add all of them
    // Do 123 loops with unrolling = 2 --> 246 iterations
    for (uint32_t i = 0; i < 123; ++i)
    {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        big_int_curve1174_square_mod(b_loc_1, b_loc_2);
        big_int_curve1174_mul_mod(r, r_loc, b_loc_1);

        big_int_curve1174_square_mod(b_loc_2, b_loc_1);
        big_int_curve1174_mul_mod(r_loc, r, b_loc_2);
    }

    // Do last iteration --> 247 ones
    big_int_curve1174_square_mod(b_loc_1, b_loc_2);
    big_int_curve1174_mul_mod(r, r_loc, b_loc_1);

    return r;
}

/**
 * \brief Calculate r := (b^((q+1)/4)) mod q
 *
 * \assumption r != b, i.e., NO ALIASING
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_q_p1_d4(BigInt *r, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_POW_Q_P1_D4);

    BIG_INT_DEFINE_PTR(b_loc_1);
    BIG_INT_DEFINE_PTR(b_loc_2);
    BIG_INT_DEFINE_PTR(r_loc);

    // (q+1)/4 = 0b111111...111110 (there are 248 ones)

    // e = 10
    big_int_curve1174_square_mod(b_loc_1, b);
    big_int_copy(r_loc, b_loc_1);

    // e = 110
    big_int_curve1174_square_mod(b_loc_2, b_loc_1);
    big_int_curve1174_mul_mod(r, r_loc, b_loc_2);

    // All the remaining bits are set to one, so we add all of them.
    // We use loop unrolling to avoid BigInt copies. 2x123 iterations -> 246 bits
    for (uint32_t i = 0; i < 123; ++i)
    {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        big_int_curve1174_square_mod(b_loc_1, b_loc_2);
        big_int_curve1174_mul_mod(r_loc, r, b_loc_1);

        big_int_curve1174_square_mod(b_loc_2, b_loc_1);
        big_int_curve1174_mul_mod(r, r_loc, b_loc_2);
    }

    return r;
}

/**
 * \brief Calculate r := (b^(q-2)) mod q
 *
 * \assumption r != b, i.e., NO ALIASING
 * \assumption r, b != NULL
 */
BigInt *big_int_curve1174_pow_q_m2(BigInt *r, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_POW_Q_M2);

    BIG_INT_DEFINE_PTR(b_loc_1);
    BIG_INT_DEFINE_PTR(b_loc_2);
    BIG_INT_DEFINE_PTR(r_loc);

    // (q-2) = 0b11111...1110101 (there are 247 ones before the suffix 0101)

    // Ensure suffix

    // e = 01
    big_int_curve1174_square_mod(b_loc_1, b);

    // e = 101
    big_int_curve1174_square_mod(b_loc_2, b_loc_1);
    big_int_curve1174_mul_mod(r_loc, b, b_loc_2);

    // e = 0101
    big_int_curve1174_square_mod(b_loc_1, b_loc_2);

    // e = 10101
    big_int_curve1174_square_mod(b_loc_2, b_loc_1);
    big_int_curve1174_mul_mod(r, r_loc, b_loc_2);

    // All the remaining bits are set to one, so we add all of them
    // Do 123 loops with unrolling = 2 --> 246 iterations
    for (uint32_t i = 0; i < 123; ++i)
    {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        big_int_curve1174_square_mod(b_loc_1, b_loc_2);
        big_int_curve1174_mul_mod(r_loc, r, b_loc_1);

        big_int_curve1174_square_mod(b_loc_2, b_loc_1);
        big_int_curve1174_mul_mod(r, r_loc, b_loc_2);
    }

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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_CHI);

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
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CURVE1174_COMPARE_TO_Q);

    // Easy cases where a is clearly larger/smaller
    if (a->size > Q_CHUNKS)
        return 1;
    if (a->size < Q_CHUNKS || a->sign)
        return -1;

    // Remaining case: a->size == Q_CHUNKS and a is positive

    // Compare to highest chunk of Q
    if (a->chunks[Q_CHUNKS - 1] < Q_MSB_CHUNK)
        return -1;
    if (a->chunks[Q_CHUNKS - 1] > Q_MSB_CHUNK)
        return 1;

    // Compare to lowest chunk of Q
    if (a->chunks[0] < Q_LSB_CHUNK)
        return -1;

    // Compare to intermediate chunks
    for (uint32_t i = Q_CHUNKS - 2; i >= 1; --i)
    {
        if (a->chunks[i] < Q_INTERMEDIATE_CHUNK)
            return -1;
    }
    return (a->chunks[0] > Q_LSB_CHUNK) ? 1 : 0;
}

/**
 * \brief Greater than (q - 1) / 2
 *
 * \returns 1 if a > (q - 1) / 2, 0 otherwise
 *
 * \assumption a != NULL
 * \assumption a is at most 8 chunks
 * \assumption a is non-negative
 */
int8_t big_int_curve1174_gt_q_m1_d2(BigInt *a)
{
    // Compare to highest chunk of Q
    if (a->size < Q_CHUNKS || a->chunks[Q_CHUNKS - 1] < Q_M1_D2_CHUNK_7)
        return 0;
    if (a->chunks[Q_CHUNKS - 1] > Q_M1_D2_CHUNK_7)
        return 1;

    // Compare to lowest chunk of Q
    if (a->chunks[0] <= Q_LSB_CHUNK)
        return 0;

    // Compare to intermediate chunks
    for (uint32_t i = Q_CHUNKS - 2; i >= 1; --i)
    {
        if (a->chunks[i] < Q_INTERMEDIATE_CHUNK)
            return 0;
    }
    return 1;
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
    for (uint32_t i = aq->size - 2; i >= 1; --i)
    {
        if (b->chunks[i] < Q_INTERMEDIATE_CHUNK)
            return 1;
    }
    return 0;
}

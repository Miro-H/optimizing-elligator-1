/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements the BigInt library to support arithmetic operations on
 * 256-bit integers.
 */

/*
 * Includes
 */
#include <stdlib.h>
#include <time.h> // for random BigInt

// header files
#include "bigint.h"
#include "debug.h"

/*
 * Macros
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*
 * Function prototypes (for internal use)
 */
BigInt *big_int_get_res(BigInt *r, BigInt *a);
BigInt *big_int_prune_leading_zeros(BigInt *r, BigInt *a);

/**
 * Global variables
 */
uint8_t do_seed_rand = 1;

/**
 * \brief if r = a:     return pointer to a; this is an in-place modification.
 *        otherwise:    copy a to r, return pointer to r
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_get_res(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_GET_RES);

    if (r == a)
        return r;
    return big_int_copy(r, a);
}

/**
 * \brief Remove leading zeros and adjust size of a BigInt
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_prune_leading_zeros(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_PRUNE_LEADING_ZEROS);

    r = big_int_get_res(r, a);

    // Find actual size of r (at least 1)
    for (int64_t i = r->size - 1; i > 0; --i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        if (r->chunks[i])
            break;

        r->size--;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    }

    return r;
}

/**
 * \brief Create BigInt from a single chunk and a sign
 *
 * \assumption r != NULL
 */
BigInt *big_int_create_from_chunk(BigInt *r, chunk_size_t chunk, uint8_t sign)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CREATE_FROM_CHUNK);

    r->sign = sign;
    r->overflow = 0;
    r->size = 1;
    r->chunks[0] = chunk;

    return r;
}

/**
 * \brief Create BigInt from a double chunk and a sign
 *
 * \assumption r != NULL
 */
BigInt *big_int_create_from_dbl_chunk(BigInt *r, dbl_chunk_size_t chunk, uint8_t sign)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CREATE_FROM_DBL_CHUNK);

    r->sign = sign;

    r->chunks[0] = chunk % BIGINT_RADIX;
    ADD_STAT_COLLECTION(BASIC_MOD)

    if (chunk >= BIGINT_RADIX) {
        r->chunks[1] = chunk / BIGINT_RADIX;
        ADD_STAT_COLLECTION(BASIC_DIV)

        r->size = 2;
    }
    else {
        r->size = 1;
    }

    return r;
}

/**
 * \brief Create BigInt from a hexadecimal string
 *
 * \assumption r, s != NULL
 * \assumption hex string s fits into BIGINT_FIXED_SIZE_INTERNAL chunks
 */
BigInt *big_int_create_from_hex(BigInt *r, char *s)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CREATE_FROM_HEX);

    size_t s_len;
    int64_t chunk_size, i;
    // buf needs space for zero byte and sign too
    char buf[BIGINT_CHUNK_HEX_SIZE + 2];
    char *s_end;
    long long parsed_int;

    s_len = strlen(s);
    s_end = s + s_len;

    // Don't count sign to integer size
    if (*s == '-') {
        s_len--;
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
    }

    // chunk_size = ceil(s_len/BIGINT_CHUNK_HEX_SIZE)
    chunk_size = (s_len + BIGINT_CHUNK_HEX_SIZE - 1) / BIGINT_CHUNK_HEX_SIZE;
    ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    ADD_STAT_COLLECTION(BASIC_DIV)

    r->overflow = 0;
    r->size = chunk_size;

    // Null terinate buffer to stop strtoll
    buf[BIGINT_CHUNK_HEX_SIZE] = 0;
    for (i = 0; i < chunk_size - 1; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        s_end -= BIGINT_CHUNK_HEX_SIZE;
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        strncpy(buf, s_end, BIGINT_CHUNK_HEX_SIZE);
        r->chunks[i] = (dbl_chunk_size_t)STR_TO_CHUNK(buf, NULL, 16);
    }

    // Parse the last (leftmost) chunk and store the sign bit
    s_len = (size_t)((uintptr_t)s_end - (uintptr_t)s);
    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

    buf[s_len] = 0;
    strncpy(buf, s, s_len);
    parsed_int = (int64_t)STR_TO_CHUNK(buf, NULL, 16);
    r->sign = parsed_int < 0;

    r->chunks[i] = (dbl_chunk_size_t)(CHUNK_ABS(parsed_int) % BIGINT_RADIX);
    ADD_STAT_COLLECTION(BASIC_MOD)
    return r;
}

/**
 * \brief Create a random BigInt
 *
 * \param r BigInt pointer should be NULL
 * \param nr_of_chunks Number of chunks in the created BigInt
 *
 * \assumption r != NULL
 * \assumption 1 <= nr_of_chunks <= BIGINT_FIXED_SIZE_INTERNAL
 */
BigInt *big_int_create_random(BigInt *r, int64_t nr_of_chunks)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CREATE_RANDOM);

    int64_t i, offset;

    if (do_seed_rand) {
        srand(time(NULL));
        do_seed_rand = 0;
    }

    r->sign = rand() % 2;
    ADD_STAT_COLLECTION(BASIC_MOD)

    r->overflow = 0;
    r->size = nr_of_chunks;

    for (i = 0; i < nr_of_chunks; i++) {
        offset = 1;
        r->chunks[i] = 0;
        while (offset < BIGINT_RADIX) {
            r->chunks[i] += ((dbl_chunk_size_t)rand()) * offset;
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
            ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)

            offset *= RAND_MAX;
            ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
        }
        r->chunks[i] %= BIGINT_RADIX;
        ADD_STAT_COLLECTION(BASIC_MOD)
    }

    return r;
}

/**
 * \brief Copy BigInt b to BigInt a, i.e., a := b
 *
 * \assumption a, b != NULL
 * \assumption b fits into a (always the case for fixed-size BigInts)
 */
BigInt *big_int_copy(BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_COPY);

    // Copy exactly as many bits as we need and not a single one more
    memcpy((void *)a, (void *)b, BIGINT_METADATA_SIZE + (b->size) * BIGINT_INTERNAL_CHUNK_BYTE);
    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
    ADD_STAT_COLLECTION(BASIC_MUL_SIZE)
    return a;
}

/**
 * \brief Print a BigInt to stdout
 *
 * \assumption a != NULL
 */
void big_int_print(BigInt *a)
{
    printf("%s0x%08" PRIx64, (a->sign == 1) ? "-" : "", a->chunks[a->size - 1]);
    if (a->size >= 2) {
        for (int64_t i = a->size - 2; i >= 0; --i) {
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            printf(" %08" PRIx64, a->chunks[i]);
        }
    }
}

/**
 * \brief Calculate r = -a
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_neg(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_NEG);

    r = big_int_get_res(r, a);
    r->sign = !r->sign;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    return r;
}

/**
 * \brief Calculate r = |a|
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_abs(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ABS);

    r = big_int_get_res(r, a);
    r->sign = 0;

    return r;
}

// === === === === === === === === === === === === === === === === === === ===

/**
 * r = a + b
 * Switches between different cases.
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_add(BigInt *r, BigInt *a, BigInt *b)
{

    BIG_INT_DEFINE_PTR(neg);

    // Use subtractions when adequate
    if (a->sign != b->sign) {
        if (a->sign == 0) {
            big_int_neg(neg, b);
            big_int_fast_sub(r, a, neg);
        }
        else {
            big_int_neg(neg, a);
            big_int_fast_sub(r, b, neg);
        }
    }
    else {
        big_int_fast_add(r, a, b);
    }
    return r;
}

/**
 * r = a - b
 * Switches between different cases.
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_sub(BigInt *r, BigInt *a, BigInt *b)
{
    BIG_INT_DEFINE_PTR(b_neg);

    if (a->sign != b->sign) {
        big_int_neg(b_neg, b);
        big_int_fast_add(r, a, b_neg);
    }
    else {
        big_int_fast_sub(r, a, b);
    }
    return r;
}

/**
 * r = a + b
 *
 * \assumption r, a, b != NULL
 * \assumption a->sign == b->sign
 * \assumption no overflow for a + b, i.e., #chunks <= BIGINT_FIXED_SIZE_INTERNAL
 */
BigInt *big_int_fast_add(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ADD);

    if (a->size == 8 && b->size == 8) {
        big_int_add_256(r, a, b);
        ADD_STAT_COLLECTION(BASIC_BITWISE)
        return r;
    }

    // Pointer used to point to a or b depending on which is larger
    BigInt *aa, *bb;

    uint8_t carry;
    int64_t i;
    dbl_chunk_size_t sum;
    uint64_t r_size;

    // Note aliasing is not an issue, since we only read potential shared
    // values _before_ we overwrite them.

    // Simplify implementation by making sure we know the larger BigInt (in
    // terms of chunks, not numerical value)
    if (a->size < b->size) {
        aa = b;
        bb = a;
    }
    else {
        aa = a;
        bb = b;
    }

    // Assertion: aa->size >= bb->size

    // First, add chunks where both have entries

    carry = 0;
    r_size = 0;
    for (i = 0; i < bb->size; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        sum = aa->chunks[i] + bb->chunks[i] + carry;
        ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
        ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

        r->chunks[i] = sum & BIGINT_CHUNK_MASK;
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        carry = sum >> BIGINT_CHUNK_SHIFT;
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        if (r->chunks[i] != 0)
            r_size = i;
    }

    // Second, finish possible remaining chunks of larger integer
    for (; i < aa->size; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        sum = aa->chunks[i] + carry;
        ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

        r->chunks[i] = sum & BIGINT_CHUNK_MASK;
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        carry = sum >> BIGINT_CHUNK_SHIFT;
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        if (r->chunks[i] != 0)
            r_size = i;
    }

    // Since both BigInts have the same sign, the result has the same sign too.
    // (ignoring overflows)

    r->sign = aa->sign;
    ADD_STAT_COLLECTION(BASIC_BITWISE)
    if (carry && i < BIGINT_FIXED_SIZE_INTERNAL) {
        r->chunks[i] = 1;
        r_size = i;
    }
    r->size = r_size + 1;
    ADD_STAT_COLLECTION(BASIC_ADD_SIZE)

    return r;
}

/**
 * r = a + b for 256-bit BigInts
 *
 * \assumption r, a, b != NULL
 * \assumption a, b are 256-bit (i.e., 8 chunks) BigInts
 * \assumption a->sign == b->sign
 * \assumption no overflow for a + b, i.e., #chunks <= BIGINT_FIXED_SIZE_INTERNAL
 */
BigInt *big_int_add_256(BigInt *r, BigInt *a, BigInt *b)
{
    dbl_chunk_size_t r_c_0, r_c_1, r_c_2, r_c_3, r_c_4, r_c_5, r_c_6, r_c_7;

    dbl_chunk_size_t a_c_0 = a->chunks[0];
    dbl_chunk_size_t a_c_1 = a->chunks[1];
    dbl_chunk_size_t a_c_2 = a->chunks[2];
    dbl_chunk_size_t a_c_3 = a->chunks[3];
    dbl_chunk_size_t a_c_4 = a->chunks[4];
    dbl_chunk_size_t a_c_5 = a->chunks[5];
    dbl_chunk_size_t a_c_6 = a->chunks[6];
    dbl_chunk_size_t a_c_7 = a->chunks[7];

    dbl_chunk_size_t b_c_0 = b->chunks[0];
    dbl_chunk_size_t b_c_1 = b->chunks[1];
    dbl_chunk_size_t b_c_2 = b->chunks[2];
    dbl_chunk_size_t b_c_3 = b->chunks[3];
    dbl_chunk_size_t b_c_4 = b->chunks[4];
    dbl_chunk_size_t b_c_5 = b->chunks[5];
    dbl_chunk_size_t b_c_6 = b->chunks[6];
    dbl_chunk_size_t b_c_7 = b->chunks[7];

    r_c_0 = a_c_0 + b_c_0;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    r_c_1 = a_c_1 + b_c_1 + (r_c_0 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r_c_2 = a_c_2 + b_c_2 + (r_c_1 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r_c_3 = a_c_3 + b_c_3 + (r_c_2 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r_c_4 = a_c_4 + b_c_4 + (r_c_3 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r_c_5 = a_c_5 + b_c_5 + (r_c_4 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r_c_6 = a_c_6 + b_c_6 + (r_c_5 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r_c_7 = a_c_7 + b_c_7 + (r_c_6 >> BIGINT_CHUNK_SHIFT);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    r->chunks[0] = r_c_0 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[1] = r_c_1 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[2] = r_c_2 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[3] = r_c_3 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[4] = r_c_4 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[5] = r_c_5 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[6] = r_c_6 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[7] = r_c_7 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    ADD_STAT_COLLECTION(BASIC_BITWISE)
    if (r_c_7 >> BIGINT_CHUNK_SHIFT) {
        r->chunks[8] = 1;
        r->size = 9;
    }
    else {
        r->size = 8;
    }

    r->sign = a->sign;
    return r;
}

/**
 * \brief Calculate r = a + b without taking carries into account
 *
 * \assumption r, a, b != NULL
 * \assumption a, b positive and has only 256 bits
 * \assumption overflow is not cleaned up
 */
BigInt *big_int_add_optimal_bound(BigInt *r, BigInt *a, BigInt *b)
{
    dbl_chunk_size_t a_c_0 = a->chunks[0];
    dbl_chunk_size_t a_c_1 = a->chunks[1];
    dbl_chunk_size_t a_c_2 = a->chunks[2];
    dbl_chunk_size_t a_c_3 = a->chunks[3];
    dbl_chunk_size_t a_c_4 = a->chunks[4];
    dbl_chunk_size_t a_c_5 = a->chunks[5];
    dbl_chunk_size_t a_c_6 = a->chunks[6];
    dbl_chunk_size_t a_c_7 = a->chunks[7];

    dbl_chunk_size_t b_c_0 = b->chunks[0];
    dbl_chunk_size_t b_c_1 = b->chunks[1];
    dbl_chunk_size_t b_c_2 = b->chunks[2];
    dbl_chunk_size_t b_c_3 = b->chunks[3];
    dbl_chunk_size_t b_c_4 = b->chunks[4];
    dbl_chunk_size_t b_c_5 = b->chunks[5];
    dbl_chunk_size_t b_c_6 = b->chunks[6];
    dbl_chunk_size_t b_c_7 = b->chunks[7];

    dbl_chunk_size_t r_c_0 = a_c_0 + b_c_0;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_1 = a_c_1 + b_c_1;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_2 = a_c_2 + b_c_2;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_3 = a_c_3 + b_c_3;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_4 = a_c_4 + b_c_4;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_5 = a_c_5 + b_c_5;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_6 = a_c_6 + b_c_6;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_7 = a_c_7 + b_c_7;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    r->chunks[0] = r_c_0;
    r->chunks[1] = r_c_1;
    r->chunks[2] = r_c_2;
    r->chunks[3] = r_c_3;
    r->chunks[4] = r_c_4;
    r->chunks[5] = r_c_5;
    r->chunks[6] = r_c_6;
    r->chunks[7] = r_c_7;

    r->size = BIGINT_FIXED_SIZE;
    r->sign = 0;

    return r;
}

/**
 * \brief Calculate r = a - b
 *
 * \assumption a->sign == b->sign
 * \assumption r, a, b != NULL
 */
BigInt *big_int_fast_sub(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SUB);

    BIG_INT_DEFINE_PTR(a_abs);
    BIG_INT_DEFINE_PTR(b_abs);
    BIG_INT_DEFINE_PTR(aa_abs);
    BIG_INT_DEFINE_PTR(bb_abs);

    int borrow;
    int64_t i;
    dbl_chunk_size_t diff;
    uint8_t do_sign_switch;
    uint64_t r_size;

    // If a, b < 0; calculate -(|a| - |b|) instead
    if (a->sign == 1) {
        big_int_abs(a_abs, a);
        big_int_abs(b_abs, b);
        do_sign_switch = 1;
    }
    else {
        a_abs = a;
        b_abs = b;
        do_sign_switch = 0;
    }

    // Note aliasing is not an issue, since we only read potential shared
    // values _before_ we overwrite them.

    // Assertion: a_abs, b_abs >= 0

    // Simplify implementation by making sure we know the numerically larger BigInt.
    // In the return statement, we account for whether we calculated a-b or b-a.
    if (big_int_compare(a_abs, b_abs) < 0) {
        aa_abs = b_abs;
        bb_abs = a_abs;
        do_sign_switch = !do_sign_switch;
    }
    else {
        aa_abs = a_abs;
        bb_abs = b_abs;
    }

    // Assertion: aa_abs >= bb_abs

    if (a->size == 8 && b->size == 8) {
        big_int_sub_256(r, aa_abs, bb_abs);
        ADD_STAT_COLLECTION(BASIC_BITWISE)
    }
    else {
        // Note an underflow sets the 1 bit at position MSB+1 of the chunk:
        // 0x0000000000000000 - 1 = 0xffffffff00000000
        //  | extra | chunk |        | extra | chunk |
        borrow = 0;
        r_size = 0;
        for (i = 0; i < bb_abs->size; ++i) {
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            diff = aa_abs->chunks[i] - bb_abs->chunks[i] - borrow;
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

            r->chunks[i] = diff & BIGINT_CHUNK_MASK;
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            borrow = (diff >> BIGINT_CHUNK_SHIFT) & 1;
            ADD_STAT_COLLECTION(BASIC_SHIFT)
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            if (r->chunks[i] != 0)
                r_size = i;
        }

        for (; i < aa_abs->size; ++i) {
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            diff = aa_abs->chunks[i] - borrow;
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

            r->chunks[i] = diff & BIGINT_CHUNK_MASK;
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            borrow = (diff >> BIGINT_CHUNK_SHIFT) & 1;
            ADD_STAT_COLLECTION(BASIC_SHIFT)
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            if (r->chunks[i] != 0)
                r_size = i;
        }

        // Assertion: borrow = 0, because aa_abs >= bb_abs
        r->size = r_size + 1;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    }

    // Because |a| >= |b| and we calculate |a| - |b|, the result is positive
    // and we never have underflow.
    r->overflow = 0;
    r->sign = do_sign_switch;

    return r;
}

/**
 * \brief Calculate r = a - b without taking borrows into account
 *
 * \assumption r, a, b != NULL
 * \assumption a->sign == b->sign
 * \assumption overflow is not cleaned up
 */
BigInt *big_int_sub_optimal_bound(BigInt *r, BigInt *a, BigInt *b)
{

    dbl_chunk_size_t a_c_0 = a->chunks[0];
    dbl_chunk_size_t a_c_1 = a->chunks[1];
    dbl_chunk_size_t a_c_2 = a->chunks[2];
    dbl_chunk_size_t a_c_3 = a->chunks[3];
    dbl_chunk_size_t a_c_4 = a->chunks[4];
    dbl_chunk_size_t a_c_5 = a->chunks[5];
    dbl_chunk_size_t a_c_6 = a->chunks[6];
    dbl_chunk_size_t a_c_7 = a->chunks[7];

    dbl_chunk_size_t b_c_0 = b->chunks[0];
    dbl_chunk_size_t b_c_1 = b->chunks[1];
    dbl_chunk_size_t b_c_2 = b->chunks[2];
    dbl_chunk_size_t b_c_3 = b->chunks[3];
    dbl_chunk_size_t b_c_4 = b->chunks[4];
    dbl_chunk_size_t b_c_5 = b->chunks[5];
    dbl_chunk_size_t b_c_6 = b->chunks[6];
    dbl_chunk_size_t b_c_7 = b->chunks[7];

    dbl_chunk_size_t r_c_0 = a_c_0 - b_c_0;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_1 = a_c_1 - b_c_1;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_2 = a_c_2 - b_c_2;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_3 = a_c_3 - b_c_3;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_4 = a_c_4 - b_c_4;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_5 = a_c_5 - b_c_5;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_6 = a_c_6 - b_c_6;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_7 = a_c_7 - b_c_7;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    r->chunks[0] = r_c_0;
    r->chunks[1] = r_c_1;
    r->chunks[2] = r_c_2;
    r->chunks[3] = r_c_3;
    r->chunks[4] = r_c_4;
    r->chunks[5] = r_c_5;
    r->chunks[6] = r_c_6;
    r->chunks[7] = r_c_7;

    r->size = BIGINT_FIXED_SIZE;
    r->sign = 0;

    return r;
}

/**
 * r = a - b for 256-bit BigInts
 *
 * \assumption a, b are both 256-bit (8-chunk) BigInts
 * \assumption r, a, b != NULL
 * \assumption a, b >= 0
 * \assumption a >= b
 */
BigInt *big_int_sub_256(BigInt *r, BigInt *a, BigInt *b)
{
    dbl_chunk_size_t a_c_0 = a->chunks[0];
    dbl_chunk_size_t a_c_1 = a->chunks[1];
    dbl_chunk_size_t a_c_2 = a->chunks[2];
    dbl_chunk_size_t a_c_3 = a->chunks[3];
    dbl_chunk_size_t a_c_4 = a->chunks[4];
    dbl_chunk_size_t a_c_5 = a->chunks[5];
    dbl_chunk_size_t a_c_6 = a->chunks[6];
    dbl_chunk_size_t a_c_7 = a->chunks[7];

    dbl_chunk_size_t b_c_0 = b->chunks[0];
    dbl_chunk_size_t b_c_1 = b->chunks[1];
    dbl_chunk_size_t b_c_2 = b->chunks[2];
    dbl_chunk_size_t b_c_3 = b->chunks[3];
    dbl_chunk_size_t b_c_4 = b->chunks[4];
    dbl_chunk_size_t b_c_5 = b->chunks[5];
    dbl_chunk_size_t b_c_6 = b->chunks[6];
    dbl_chunk_size_t b_c_7 = b->chunks[7];

    dbl_chunk_size_t r_c_0 = a_c_0 - b_c_0;
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

    dbl_chunk_size_t r_c_1 = a_c_1 - b_c_1 - ((r_c_0 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    dbl_chunk_size_t r_c_2 = a_c_2 - b_c_2 - ((r_c_1 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    dbl_chunk_size_t r_c_3 = a_c_3 - b_c_3 - ((r_c_2 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    dbl_chunk_size_t r_c_4 = a_c_4 - b_c_4 - ((r_c_3 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    dbl_chunk_size_t r_c_5 = a_c_5 - b_c_5 - ((r_c_4 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    dbl_chunk_size_t r_c_6 = a_c_6 - b_c_6 - ((r_c_5 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    dbl_chunk_size_t r_c_7 = a_c_7 - b_c_7 - ((r_c_6 >> BIGINT_CHUNK_SHIFT) & 1);
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_0 = r_c_0 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_1 = r_c_1 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_2 = r_c_2 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_3 = r_c_3 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_4 = r_c_4 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_5 = r_c_5 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_6 = r_c_6 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r_c_7 = r_c_7 & BIGINT_CHUNK_MASK;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->chunks[0] = r_c_0;
    r->chunks[1] = r_c_1;
    r->chunks[2] = r_c_2;
    r->chunks[3] = r_c_3;
    r->chunks[4] = r_c_4;
    r->chunks[5] = r_c_5;
    r->chunks[6] = r_c_6;
    r->chunks[7] = r_c_7;

    r->size = 0;
    for (uint32_t i = 0; i < BIGINT_FIXED_SIZE; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        if (r->chunks[i])
            r->size = i;
    }
    r->size += 1;
    ADD_STAT_COLLECTION(BASIC_ADD_SIZE)

    return r;
}

/**
 * \brief Calculate r = a * b
 *
 * \assumption r, a, b != NULL
 * \assumption a->size + b->size <= BIGINT_FIXED_SIZE
 * \assumption b is positive
 * \assumption no aliasing
 */
BigInt *big_int_mul_single_chunk(BigInt *r, BigInt *a, dbl_chunk_size_t b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MUL_SINGLE_CHUNK);

    int64_t j;
    dbl_chunk_size_t carry;

    r->sign = a->sign ^ 0;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    // Multiply and add chunks
    carry = 0;
    for (j = 0; j < a->size; ++j) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        carry += a->chunks[j] * b;
        ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
        ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)

        r->chunks[j] = carry & BIGINT_RADIX_FOR_MOD;
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        carry >>= BIGINT_CHUNK_BIT_SIZE;
        ADD_STAT_COLLECTION(BASIC_SHIFT)
    }

    if (carry > 0) {
        r->chunks[a->size] = carry;
        r->size = a->size + 1;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    }
    else {
        r->size = a->size;
    }

    return r;
}

/**
 * \brief Calculate r = a^2
 *
 * \assumption r != a, i.e., NO ALIASING
 * \assumption r, a != NULL
 * \assumption 2 * (a->size) <= BIGINT_FIXED_SIZE_INTERNAL
 */
BigInt *big_int_square(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SQUARED);

    int64_t i, j;
    unsigned __int128 carry;

    // Set BigInt to all zeros (initializes chunks, sign, and overflow)
    *r = (BigInt){0};
    r->size = 2 * a->size;
    ADD_STAT_COLLECTION(BASIC_MUL_SIZE)

    for (i = 0; i < a->size; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        // shortcut for zero chunk
        if (a->chunks[i] == 0) {
            r->chunks[i + a->size] = 0;
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        }
        else {
            // Multiply and add chunks
            carry = a->chunks[i] * a->chunks[i] + r->chunks[i + i];
            ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            r->chunks[i + i] = (dbl_chunk_size_t)(carry & BIGINT_RADIX_FOR_MOD_128);
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            ADD_STAT_COLLECTION(BASIC_BITWISE)

            carry >>= BIGINT_CHUNK_BIT_SIZE;
            ADD_STAT_COLLECTION(BASIC_SHIFT)

            for (j = i + 1; j < a->size; j++) {
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

                carry += (unsigned __int128)2 * a->chunks[j] * a->chunks[i] + r->chunks[i + j];
                ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
                ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
                ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

                r->chunks[i + j] = (dbl_chunk_size_t)(carry & BIGINT_RADIX_FOR_MOD_128);
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                ADD_STAT_COLLECTION(BASIC_BITWISE)

                carry >>= BIGINT_CHUNK_BIT_SIZE;
                ADD_STAT_COLLECTION(BASIC_SHIFT)
            }
            r->chunks[i + a->size] = carry;
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        }
    }

    // Remove leading zeros
    for (i = r->size - 1; i > 0; --i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        if (r->chunks[i])
            break;

        r->size--;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    }

    return r;
}

// === === === === === === === === === === === === === === === === === === ===

/**
 * \brief Calculate r = a * b
 *
 * \assumption r, a, b != NULL
 * \assumption a->size + b->size <= BIGINT_FIXED_SIZE_INTERNAL
 * \assumption a != b, i.e., no aliasing
 */
BigInt *big_int_mul(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MUL);

    int64_t i, j;
    dbl_chunk_size_t carry;

    *r = (BigInt){0};

    r->sign = a->sign ^ b->sign;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    r->size = a->size + b->size;
    ADD_STAT_COLLECTION(BASIC_ADD_SIZE)

    for (i = 0; i < b->size; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        // shortcut for zero chunk
        if (b->chunks[i] == 0) {
            r->chunks[i + a->size] = 0;
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        }
        else {
            // Multiply and add chunks
            carry = 0;
            for (j = 0; j < a->size; ++j) {
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

                carry += a->chunks[j] * b->chunks[i] + r->chunks[i + j];
                ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
                ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

                r->chunks[i + j] = carry & BIGINT_RADIX_FOR_MOD;
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                ADD_STAT_COLLECTION(BASIC_BITWISE)

                carry >>= BIGINT_CHUNK_BIT_SIZE;
                ADD_STAT_COLLECTION(BASIC_SHIFT)

                r->chunks[i + a->size] = carry;
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            }
        }
    }
    // Prune leading zeros
    for (int64_t i = r->size - 1; i > 0; --i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        if (r->chunks[i])
            break;

        r->size--;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    }

    return r;
}

/**
 * \brief Calculate quotient q and remainder r, such that: a = q * b + r
 *        We round numbers towards 0, e.g., -13/8 = (-1, -5)
 *
 * \param r if r is not NULL, it will be set to the remainder.
 * \returns pointer to q
 *
 * \assumption q, a, b != NULL
 */
BigInt *big_int_div_rem(BigInt *q, BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DIV_REM);

    dbl_chunk_size_t a_tmp, b_tmp, tmp, q_c, r_c;
    uint8_t q_sign;
    uint64_t factor;
    int64_t q_idx, a_idx, i;

    BIG_INT_DEFINE_PTR(a_abs);
    BIG_INT_DEFINE_PTR(b_abs);
    BIG_INT_DEFINE_PTR(a_loc);
    BIG_INT_DEFINE_PTR(a_loc_2);
    BIG_INT_DEFINE_PTR(b_loc);
    BIG_INT_DEFINE_PTR(b_loc_2);
    BIG_INT_DEFINE_PTR(a_part);
    BIG_INT_DEFINE_PTR(q_c_bigint);
    BIG_INT_DEFINE_PTR(qb);
    BIG_INT_DEFINE_PTR(r_loc);

    BIG_INT_DEFINE_PTR_ZEROED(radix_pow);

    if (big_int_is_zero(b))
        FATAL("Division by zero!\n");

    // DEBUG_BIGINT(a, "div_rem; a = ");
    // DEBUG_BIGINT(b, "div_rem; b = ");

    /*
     * Special cases
     */
    big_int_abs(a_abs, a);
    big_int_abs(b_abs, b);

    // zero dividend
    if (big_int_is_zero(a)) {
        big_int_create_from_chunk(q, 0, 0);
        big_int_create_from_chunk(r_loc, 0, 0);
    }
    // divisor larger than dividend
    else if (big_int_compare(b_abs, a_abs) == 1) {
        big_int_create_from_chunk(q, 0, 0);

        // Save unsigned rest, account for signs later
        big_int_copy(r_loc, a);
        big_int_abs(r_loc, r_loc);
    }
    // Simple case for small BigInts, just use normal C division (but round
    // towards -inf at the end)
    else if (a->size == 1) {
        big_int_create_from_chunk(q, a->chunks[0] / b->chunks[0], 0);
        ADD_STAT_COLLECTION(BASIC_DIV)

        q->sign = a->sign ^ b->sign;
        big_int_create_from_chunk(r_loc, a->chunks[0] % b->chunks[0], 0);
        ADD_STAT_COLLECTION(BASIC_MOD)
    }
    // Since we do operations on double chunks, we can also do the shortcut for length 2
    else if (a->size == 2) {
        a_tmp = a->chunks[1] * BIGINT_RADIX + a->chunks[0];
        ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
        ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

        b_tmp = b->chunks[0];
        if (b->size == 2) {
            b_tmp += b->chunks[1] * BIGINT_RADIX;
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
            ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
        }

        q_sign = a->sign ^ b->sign;
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        q = big_int_create_from_dbl_chunk(q, a_tmp / b_tmp, q_sign);
        ADD_STAT_COLLECTION(BASIC_DIV)

        r_loc = big_int_create_from_dbl_chunk(r_loc, a_tmp % b_tmp, 0);
        ADD_STAT_COLLECTION(BASIC_MOD)
    }
    else
    {
        // Below: Division for a->size > 2

        q->sign = a->sign ^ b->sign;
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        q->size = a->size - b->size + 1;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)

        // Preserve values of a, b
        big_int_copy(a_loc, a);
        big_int_copy(b_loc, b);

        // Operate on absolute values, account for signs at the end
        big_int_abs(a_loc, a_loc);
        big_int_abs(b_loc, b_loc);

        // Normalize: find factor such that b->chunks[b->size - 1] >= BIGINT_RADIX/2
        factor = 0;
        tmp = b_loc->chunks[b->size - 1];
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
        while (2 * tmp < BIGINT_RADIX) {
            ++factor;
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            tmp <<= 1;
            ADD_STAT_COLLECTION(BASIC_SHIFT)
        }

        if (factor > 0) {
            big_int_sll_small(a_loc_2, a_loc, factor);
            a_loc = a_loc_2;
            big_int_sll_small(b_loc_2, b_loc, factor);
            b_loc = b_loc_2;
        }

        if (a_loc->size == a->size) {
            // Special case where we actually have a zero in the MSB, just to make
            // subsequent ops easier
            a_loc->chunks[a->size] = 0;
            a_loc->size += 1;
            ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
        }

        radix_pow->chunks[b->size] = 1;
        radix_pow->size = b->size + 1;
        ADD_STAT_COLLECTION(BASIC_ADD_SIZE)

        // Calculate quotient digit by digit
        for (q_idx = q->size - 1; q_idx >= 0; --q_idx) {
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            // Calculate quotient and remainder of
            // a->chunks[q_idx : q_idx + b->size] / b_loc

            a_idx = q_idx + b->size;
            ADD_STAT_COLLECTION(BASIC_ADD_SIZE)

            q_c = a_loc->chunks[a_idx] * BIGINT_RADIX + a_loc->chunks[a_idx - 1];
            ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
            ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            r_c = q_c % b_loc->chunks[b->size - 1];
            ADD_STAT_COLLECTION(BASIC_MOD)
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            q_c /= b_loc->chunks[b->size - 1];
            ADD_STAT_COLLECTION(BASIC_DIV)
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

            do {
                ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
                ADD_STAT_COLLECTION(BASIC_MUL_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
                ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
                if (q_c >= BIGINT_RADIX
                    || (b->size > 1 && q_c * b_loc->chunks[b->size - 2] >
                        BIGINT_RADIX * r_c + a_loc->chunks[a_idx - 2]))
                {
                    --q_c;
                    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)

                    r_c += b_loc->chunks[b_loc->size - 1];
                    ADD_STAT_COLLECTION(BASIC_ADD_CHUNK)
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                }
                else {
                    break;
                }
            } while (r_c < BIGINT_RADIX);

            // Multiply and subtract
            // XXX: this entire part can surely be done more efficiently. However,
            // we avoid using divrem in Elligator, so it does not matter for us.
            for (i = 0; i <= b_loc->size; ++i) {
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                if (q_idx + i >= a_loc->size)
                    break;

                a_part->chunks[i] = a_loc->chunks[q_idx + i];
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            }
            a_part->size = i;
            a_part->sign = 0;

            big_int_create_from_chunk(q_c_bigint, q_c, 0);
            big_int_sub(qb, a_part, big_int_mul(qb, q_c_bigint, b_loc));

            // This is a very unlikely case (cf. step D6 in D. Knuth's book section 4.3)
            if (qb->sign == 1)
            {
                // WARNING("Rare case in div_rem triggered\n");
                big_int_add(qb, qb, radix_pow);

                --q_c;
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                // NOTE: we intentionally ignore the overflow in a_part, it cancels
                // out with the borrow that we ignored from the previous add.
                big_int_add(qb, qb, b_loc);

                for (i = 0; i <= b->size; ++i) {
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                    if (q_idx + i >= a_loc->size) break;

                    a_loc->chunks[q_idx + i] = qb->chunks[i];
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                }
            }
            else {
                for (i = 0; i <= b->size; ++i) {
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                    if (q_idx + i >= a_loc->size) break;

                    a_loc->chunks[q_idx + i] = qb->chunks[i];
                    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
                }
            }

            q->chunks[q_idx] = q_c;
        }

        // Unnormalize
        a_loc->size = b->size;
        r_loc = big_int_copy(r_loc, a_loc);
        big_int_srl_small(r_loc, r_loc, factor);
        big_int_prune_leading_zeros(r_loc, r_loc);

        big_int_prune_leading_zeros(q, q);
    }

    // Round towards -inf if either operand is negative
    ADD_STAT_COLLECTION(BASIC_BITWISE)
    ADD_STAT_COLLECTION(BASIC_BITWISE)
    if (a->sign ^ b->sign && !big_int_is_zero(r_loc)) {
        big_int_sub(q, q, big_int_one);
        big_int_abs(b_loc, b);
        big_int_sub(r_loc, b_loc, r_loc);
    }
    // If b is negative, flip the sign of the remainder to maintain the
    // invariant q * b + r = a
    if (b->sign)
        big_int_neg(r_loc, r_loc);

    if (r)
        big_int_copy(r, r_loc);

    return q;
}

/**
 * \brief Calculate r = a // b (integer division)
 *
 * \assumption r, a, b != NULL
 */
BigInt *big_int_div(BigInt *q, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DIV);

    return big_int_div_rem(q, NULL, a, b);
}

/**
 * \brief Calculate r = a << shift for "small" shifts (not BigInt shifts)
 *
 * \assumption r, a != NULL
 * \assumption r != a: no aliasing
 * \assumption a->size + shift / BIGINT_CHUNK_BIT_SIZE < BIGINT_FIXED_SIZE_INTERNAL
 */
BigInt *big_int_sll_small(BigInt *r, BigInt *a, uint64_t shift)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SLL_SMALL);

    dbl_chunk_size_t carry;
    int64_t i, r_idx;

    // Set the lowest chunks to zero
    for (r_idx = 0; r_idx < (int64_t)(shift / BIGINT_CHUNK_BIT_SIZE); ++r_idx) {
        ADD_STAT_COLLECTION(BASIC_DIV)
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        r->chunks[r_idx] = 0;
    }

    // Shift the other chunks by the chunk internal shift
    shift %= BIGINT_CHUNK_BIT_SIZE;
    ADD_STAT_COLLECTION(BASIC_MOD)
    carry = 0;
    for (i = 0; i < a->size; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        carry = (a->chunks[i] << shift) | carry;
        ADD_STAT_COLLECTION(BASIC_SHIFT)
        ADD_STAT_COLLECTION(BASIC_BITWISE)

        r->chunks[r_idx] = carry % BIGINT_RADIX;
        ADD_STAT_COLLECTION(BASIC_MOD)

        carry /= BIGINT_RADIX;
        ADD_STAT_COLLECTION(BASIC_DIV)

        ++r_idx;
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
    }

    r->overflow = 0;
    r->sign = a->sign;

    // Add the last block if there is a carry from the MSB block
    if (carry) {
        if (r_idx < BIGINT_FIXED_SIZE_INTERNAL) {
            r->chunks[r_idx] = carry;
            ++r_idx;
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        }
        else {
            WARNING("sll caused overflow!\n");
            r->overflow = 1;
        }
    }

    r->size = r_idx;

    return r;
}

/**
 * \brief Calculate r = a >> shift for "small" shifts (not BigInt shifts)
 *
 * \assumption r, a != NULL
 */
BigInt *big_int_srl_small(BigInt *r, BigInt *a, uint64_t shift)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SRL_SMALL);

    BIG_INT_DEFINE_PTR(last_bit_bigint);

    dbl_chunk_size_t carry, last_bit;
    int64_t i, a_idx;

    // Special case where we shift away all bits
    ADD_STAT_COLLECTION(BASIC_DIV)
    if (shift / BIGINT_CHUNK_BIT_SIZE >= a->size) {

        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        ADD_STAT_COLLECTION(BASIC_DIV)
        // Special case for rounding towards -inf
        if (a->sign && (shift - 1) / BIGINT_CHUNK_BIT_SIZE < a->size) {
            big_int_create_from_chunk(r,
                a->chunks[a->size - 1] >> (((shift - 1) % BIGINT_CHUNK_BIT_SIZE)),
                1);
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            ADD_STAT_COLLECTION(BASIC_SHIFT)
            ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
            ADD_STAT_COLLECTION(BASIC_MOD)
        }
        else
        {
            big_int_create_from_chunk(r, 0, 0);
        }
        return r;
    }

    r->size = a->size - shift / BIGINT_CHUNK_BIT_SIZE;
    ADD_STAT_COLLECTION(BASIC_ADD_SIZE)
    ADD_STAT_COLLECTION(BASIC_DIV)

    // Take the chunks of a that are not all completely shifted away
    a_idx = (int64_t)(shift / BIGINT_CHUNK_BIT_SIZE);
    ADD_STAT_COLLECTION(BASIC_DIV)

    shift %= BIGINT_CHUNK_BIT_SIZE;
    ADD_STAT_COLLECTION(BASIC_MOD)

    // Store the last bit that is shifted away to implement rounding towards -inf
    last_bit = (a->chunks[a_idx] >> (shift - 1)) & 1;
    ADD_STAT_COLLECTION(BASIC_SHIFT)
    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    // Go upwards to avoid aliasing, since we always write r[i] and read
    // a[a_idx+1], where i < a_idx + 1.
    for (i = 0; i < r->size - 1; ++i) {
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

        carry = (a->chunks[a_idx + 1] << (BIGINT_CHUNK_BIT_SIZE - shift)) % BIGINT_RADIX;
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        ADD_STAT_COLLECTION(BASIC_SHIFT)
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
        ADD_STAT_COLLECTION(BASIC_MOD)

        r->chunks[i] = carry | (a->chunks[a_idx] >> shift);
        ADD_STAT_COLLECTION(BASIC_BITWISE)
        ADD_STAT_COLLECTION(BASIC_SHIFT)

        ++a_idx;
        ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
    }
    // Write MSB entry
    r->chunks[r->size - 1] = a->chunks[a_idx] >> shift;
    ADD_STAT_COLLECTION(BASIC_ADD_OTHER)
    ADD_STAT_COLLECTION(BASIC_SHIFT)

    // Add last bit (round towards -inf)
    if (a->sign && last_bit) {
        big_int_create_from_chunk(last_bit_bigint, last_bit, 0);
        ADD_STAT_COLLECTION(BASIC_BITWISE)
        big_int_sub(r, r, last_bit_bigint);
    }

    r->overflow = 0;
    r->sign = a->sign;

    return big_int_prune_leading_zeros(r, r);
}

/**
 * \brief Calculate r := a mod q
 *
 * \assumption r, a, q != NULL
 */
BigInt *big_int_mod(BigInt *r, BigInt *a, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MOD);

    BIG_INT_DEFINE_PTR(tmp);

    big_int_div_rem(tmp, r, a, q);

    return r;
}

/**
 * \brief Calculate r := (a + b) mod q
 *
 * \assumption r, a, b, q != NULL
 */
BigInt *big_int_add_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ADD_MOD);

    BIG_INT_DEFINE_PTR(r_loc);

    big_int_add(r_loc, a, b);
    big_int_mod(r, r_loc, q);

    return r;
}

/**
 * \brief Calculate r := (a - b) mod q
 *
 * \assumption r, a, b, q != NULL
 */
BigInt *big_int_sub_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SUB_MOD);

    BIG_INT_DEFINE_PTR(r_loc);

    big_int_sub(r_loc, a, b);
    big_int_mod(r, r_loc, q);

    return r;
}

/**
 * \brief Calculate r := (a * b) mod q
 *
 * \assumption r, a, b, q != NULL
 */
BigInt *big_int_mul_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MUL_MOD);

    BIG_INT_DEFINE_PTR(r_loc);

    big_int_mul(r_loc, a, b);
    big_int_mod(r, r_loc, q);

    return r;
}

/**
 * \brief Calculate r := (a * b) mod q
 *
 * \assumption r, a, b, q != NULL
 */
BigInt *big_int_square_mod(BigInt *r, BigInt *a, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SQUARED_MOD);

    BIG_INT_DEFINE_PTR(r_loc);

    big_int_square(r_loc, a);
    big_int_mod(r, r_loc, q);

    return r;
}

/**
 * \brief Calculate r := (a * b^-1) mod q
 *
 * \assumption r, a, b, q != NULL
 */
BigInt *big_int_div_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DIV_MOD);

    BIG_INT_DEFINE_PTR(r_loc);

    // Write to local copy to be save against pointer reuse
    big_int_inv(r_loc, b, q);
    big_int_mul_mod(r, a, r_loc, q);

    return r;
}

/**
 * \brief Calculate r := a^-1 mod q (the inverse of a)
 *
 * \assumption r, a, q != NULL
 * \assumption a is invertible mod q, i.e. res.g = 1
 */
BigInt *big_int_inv(BigInt *r, BigInt *a, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_INV);

    EgcdResult res;

    big_int_egcd(&res, a, q);

    // set r = x mod q
    big_int_mod(r, &res.x, q);

    return r;
}

/**
 * \brief Calculate r := (b^e) mod q
 *
 * \assumption r, b, e, q != NULL
 *
 * NOTE: there is an optimized pow function for Elligator in bigint_curve1174,
 * that we use instead. Not all optimizations were ported back to this function.
 */
BigInt *big_int_pow(BigInt *r, BigInt *b, BigInt *e, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_POW);

    BIG_INT_DEFINE_PTR(e_loc);
    BIG_INT_DEFINE_PTR(b_loc1);
    BIG_INT_DEFINE_PTR(b_loc2);
    BIG_INT_DEFINE_PTR(r_loc1);

    BIG_INT_DEFINE_FROM_CHUNK(r_loc2, 0, 1);

    BigInt *r_loc_tmp;

    big_int_copy(e_loc, e);
    big_int_copy(b_loc1, b);

    while (big_int_compare(e_loc, big_int_zero) > 0) {
        // If power is odd
        if (big_int_is_odd(e_loc)) {
            big_int_mul_mod(r_loc1, r_loc2, b_loc1, q);
            r_loc_tmp = r_loc1;
            r_loc1 = r_loc2;
            r_loc2 = r_loc_tmp;
        }

        big_int_srl_small(e_loc, e_loc, 1);

        if (big_int_is_zero(e_loc))
            break;

        big_int_square_mod(b_loc2, b_loc1, q);

        // ------ unroll ------
        if (big_int_is_odd(e_loc)) {
            big_int_mul_mod(r_loc1, r_loc2, b_loc2, q);
            r_loc_tmp = r_loc1;
            r_loc1 = r_loc2;
            r_loc2 = r_loc_tmp;
        }

        big_int_srl_small(e_loc, e_loc, 1);

        if (!big_int_is_zero(e_loc))
            big_int_square_mod(b_loc1, b_loc2, q);
    }

    // This copy could be saved if we assume no aliasing, but we don't use
    // this function for Elligator anymore internally.
    return big_int_copy(r, r_loc2);
}

/**
 * \brief Returns true if the BigInt is zero
 *
 * \assumption a != NULL
 */
int8_t big_int_is_zero(BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_IS_ZERO);

    return a->size == 1 && a->chunks[0] == 0;
    ADD_STAT_COLLECTION(BASIC_BITWISE)
}

/**
 * \brief Returns true if the BigInt is an odd number
 *
 * \assumption a != NULL
 */
int8_t big_int_is_odd(BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_IS_ODD);

    return (a->size > 0) && (a->chunks[0] & 1);
    ADD_STAT_COLLECTION(BASIC_BITWISE)
}

/**
 * \brief Calculate a == b.
 * \returns r, where a==b: r=0, a<b: r<0, a>b: r>0
 *
 * \assumption a, b != NULL
 */
int8_t big_int_compare(BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_COMPARE);

    if (a->size == b->size) {
        if (big_int_is_zero(b)) {
            if (big_int_is_zero(a)) {
                return 0;
            }
            return (a->sign == 1) ? -1 : 1;
        }
        else if (big_int_is_zero(a)) {
            return (b->sign == 1) ? 1 : -1;
        }
        else if (a->sign != b->sign) {
            return (a->sign == 1) ? -1 : 1;
        }
        else {
            // Actually compare all the chunks, from the largest to the smallest
            for (int64_t i = a->size - 1; i >= 0; --i) {
                ADD_STAT_COLLECTION(BASIC_ADD_OTHER)

                if (a->chunks[i] != b->chunks[i]) {
                    if (a->sign == 1)
                        return (a->chunks[i] > b->chunks[i]) ? -1 : 1;
                    else
                        return (a->chunks[i] > b->chunks[i]) ? 1 : -1;
                }
            }
            return 0;
        }
    }
    else
    {
        if (a->sign != b->sign)
            return (a->sign == 1) ? -1 : 1;

        if (a->sign == 1)
            return (a->size > b->size) ? -1 : 1;
        else
            return (a->size > b->size) ? 1 : -1;
    }
}

/**
 * \brief Calculate the greatest common divisor using the extended Euclidean
 *        algorithm (iterative version).
 * \returns Pointer to EgcdResult (x, y, g), where x * a + y * b = g
 *          and g is the GCD.
 *
 * \assumption r, a, b != NULL
 */
EgcdResult *big_int_egcd(EgcdResult *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_EGCD);

    BIG_INT_DEFINE_PTR(q);
    BIG_INT_DEFINE_PTR(a_loc);
    BIG_INT_DEFINE_PTR(x1);
    BIG_INT_DEFINE_PTR(y1);
    BIG_INT_DEFINE_PTR(tmp);
    BIG_INT_DEFINE_PTR(tmp2);

    // Avoid copying intermediate results to final result by using the latter
    // from the start
    BigInt *b_loc = &(r->g);
    BigInt *x0 = &(r->x);
    BigInt *y0 = &(r->y);

    if (big_int_is_zero(a) && big_int_is_zero(b)) {
        big_int_create_from_chunk(x0, 0, 0);
        ADD_STAT_COLLECTION(BASIC_BITWISE)
        big_int_create_from_chunk(y0, 0, 0);
        big_int_create_from_chunk(b_loc, 0, 0);
        return r;
    }

    big_int_create_from_chunk(x0, 0, 0);
    big_int_create_from_chunk(x1, 1, 0);
    big_int_create_from_chunk(y0, 1, 0);
    big_int_create_from_chunk(y1, 0, 0);

    big_int_copy(a_loc, a);
    big_int_copy(b_loc, b);

    // Take the GCD of positive numbers, then just flig the signs of x, y at the end
    big_int_abs(a_loc, a_loc);
    big_int_abs(b_loc, b_loc);

    while (big_int_compare(a_loc, big_int_zero) != 0) {
        big_int_div_rem(q, tmp, b_loc, a_loc);
        big_int_copy(b_loc, a_loc);
        big_int_copy(a_loc, tmp);

        big_int_copy(tmp, y1);
        big_int_mul(tmp2, y1, q);
        big_int_sub(y1, y0, tmp2);
        big_int_copy(y0, tmp);

        big_int_copy(tmp, x1);
        big_int_mul(tmp2, x1, q);
        big_int_sub(x1, x0, tmp2);
        big_int_copy(x0, tmp);
    }

    // Account for signs of a and b
    x0->sign ^= a->sign;
    ADD_STAT_COLLECTION(BASIC_BITWISE)
    y0->sign ^= b->sign;
    ADD_STAT_COLLECTION(BASIC_BITWISE)

    return r;
}

/**
 * \brief Calculate the Chi function chi(t) = t**((q-1)/2) mod q
 * \returns 0 if t = 0, 1 if t is a non-zero square, -1 otherwise
 *
 * \assumption r, t, q != NULL
 * \assumption q is prime
 *
 * NOTE: there is an optimized Chi function for Elligator in bigint_curve1174,
 * that we use instead. Not all optimizations were ported back to this function.
 */
BigInt *big_int_chi(BigInt *r, BigInt *t, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CHI);

    BIG_INT_DEFINE_PTR(e);

    if (big_int_compare(t, big_int_zero) == 0) {
        // XXX: [Optimization] change return value to int8_t
        big_int_create_from_chunk(r, 0, 0);
        return r;
    }

    big_int_copy(e, q);
    big_int_srl_small(e, big_int_sub(e, e, big_int_one), 1);

    big_int_pow(r, t, e, q);

    ADD_STAT_COLLECTION(BASIC_BITWISE)
    ADD_STAT_COLLECTION(BASIC_BITWISE)
    if (!big_int_compare(r, big_int_zero) || !big_int_compare(r, big_int_one))
        return r;

    // XXX: [Optimization] change return value to int8_t
    big_int_create_from_chunk(r, 1, 1); // r = -1
    return r;
}

// This function has to be at the end of this file for the type auto-generation
// to work.
void reset_stats()
{
    for (int64_t i = 0; i < BIGINT_TYPE_LAST; ++i)
        big_int_stats[i] = 0;
}

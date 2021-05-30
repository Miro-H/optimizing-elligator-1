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

/**
 * Global variables
 */
uint8_t do_seed_rand = 1;

/**
 * \brief Allocate a BigInt of the given size
 */
BigInt *big_int_alloc(uint64_t size)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ALLOC);

    BigInt *a;

    a = (BigInt *) malloc(sizeof(BigInt));
    if (!a)
        FATAL("Failed to malloc BigInt.\n");

    a->chunks = (dbl_chunk_size_t *) malloc(size * sizeof(dbl_chunk_size_t));
    if (!a->chunks) {
        free(a);
        FATAL("Failed to malloc %" PRIu64 " chunks for malloc.\n", size);
    }
    a->alloc_size = size;

    return a;
}

/**
 * \brief Allocate a BigInt of the given size
 */
BigInt *big_int_calloc(uint64_t size)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CALLOC);

    BigInt *a;

    a = (BigInt *) calloc(1, sizeof(BigInt));
    if (!a)
        FATAL("Failed to calloc BigInt.\n");

    a->chunks = (dbl_chunk_size_t *) calloc(size, sizeof(dbl_chunk_size_t));
    if (!a->chunks) {
        free(a);
        FATAL("Failed to calloc %" PRIu64 " chunks for malloc.\n", size);
    }
    a->alloc_size = size;

    return a;
}


/**
 * \brief if r = NULL:  allocate new BigInt, return pointer to new BigInt
 *        if r = a:     return pointer to a; this is an in-place modification.
 *        otherwise:    copy a to r, return pointer to r
 */
BigInt *big_int_get_res(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_GET_RES);

    if (!r)
        return big_int_duplicate(a);
    if (r == a)
        return r;
    return big_int_copy(r, a);
}


/**
 * \brief Remove leading zeros and adjust size of a BigInt
 */
BigInt *big_int_prune_leading_zeros(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_PRUNE_LEADING_ZEROS);

    r = big_int_get_res(r, a);

    // Find actual size of r (at least 1)
    for (int64_t i = r->size - 1; i > 0; --i) {
        if (r->chunks[i])
            break;
        r->size--;
    }

    return r;
}


/**
 * \brief Create BigInt from 64-bit integer
 */
BigInt *big_int_create_from_chunk(BigInt *r, chunk_size_t chunk, uint8_t sign)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_CREATE);

    // NOTE: Currently, all BigInts have size BIGINT_FIXED_SIZE
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    r->chunks[0] = chunk;
    r->sign      = sign;
    r->overflow  = 0;
    r->size      = 1;

    return r;
}


/**
 * \brief Create BigInt from a double chunks and a sign
 */
BigInt *big_int_create_from_dbl_chunk(BigInt *r, dbl_chunk_size_t chunk,
    uint8_t sign)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CREATE_FROM_DBL_CHUNK);

    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    r->sign = sign;

    r->chunks[0] = chunk % BIGINT_RADIX;

    if (chunk >= BIGINT_RADIX) {
        r->chunks[1] = chunk / BIGINT_RADIX;
        r->size = 2;
    }
    else {
        r->size = 1;
    }

    return r;
}


/**
 * \brief Create BigInt from a hexadecimal string
 */
BigInt *big_int_create_from_hex(BigInt *r, char* s)
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
    if (*s == '-')
        s_len--;

    // chunk_size = ceil(s_len/BIGINT_CHUNK_HEX_SIZE)
    chunk_size = (s_len + BIGINT_CHUNK_HEX_SIZE - 1) / BIGINT_CHUNK_HEX_SIZE;

    // Check if the given string is too large for our BigInts
    if (chunk_size > BIGINT_FIXED_SIZE)
        FATAL("Integer %s is larger than %" PRIu64 " bytes and currently not supported.\n",
            s, BIGINT_FIXED_SIZE * sizeof(chunk_size_t));

    // NOTE: Currently, all BigInts have size BIGINT_FIXED_SIZE. For dynamic arbitrary
    // precision integers, we'd need to check that r has the right size!
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);
    else if (r->alloc_size < chunk_size)
        FATAL("The given output BigInt is not large enough to store %s!\n", s);

    r->overflow  = 0;
    r->size      = chunk_size;

    // Null terinate buffer to stop strtoll
    buf[BIGINT_CHUNK_HEX_SIZE] = 0;
    for (i = 0; i < chunk_size - 1; ++i) {
        s_end -= BIGINT_CHUNK_HEX_SIZE;
        strncpy(buf, s_end, BIGINT_CHUNK_HEX_SIZE);

        r->chunks[i] = (dbl_chunk_size_t) STR_TO_CHUNK(buf, NULL, 16);
    }

    // Parse the last (leftmost) chunk and store the sign bit
    s_len = (size_t) ((uintptr_t) s_end - (uintptr_t) s);
    buf[s_len] = 0;
    strncpy(buf, s, s_len);
    parsed_int = (int64_t) STR_TO_CHUNK(buf, NULL, 16);
    r->sign = parsed_int < 0;
    r->chunks[i] = (dbl_chunk_size_t) (CHUNK_ABS(parsed_int) % BIGINT_RADIX);
    return r;
}


/**
 * \brief Create a random BigInt
 *
 * \param r BigInt pointer should be NULL
 * \param nr_of_chunks Number of chunks in the created BigInt
 */
BigInt *big_int_create_random(BigInt *r, int64_t nr_of_chunks)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CREATE_RANDOM);

    int64_t i, offset;

    // NOTE: For arbitrary sized BigInts, this would allocate at least nr_of_chunks
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    if (do_seed_rand) {
        srand(time(NULL));
        do_seed_rand = 0;
    }

    r->sign = rand() % 2;
    r->overflow  = 0;
    r->size      = nr_of_chunks;

    for(i = 0; i < nr_of_chunks; i++)
    {
        offset = 1;
        r->chunks[i] = 0;
        while (offset < BIGINT_RADIX) {
            r->chunks[i] += ((dbl_chunk_size_t) rand()) * offset;
            offset *= RAND_MAX;
        }
        r->chunks[i] %= BIGINT_RADIX;
    }

    return r;
}



/**
 * \brief Free a BigInt
 */
void big_int_destroy(BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DESTROY);

    if (a->alloc_size > 0)
        free(a->chunks);
    free(a);
}


/**
 * \brief Copy BigInt b to BigInt a, i.e., a := b
 */
BigInt *big_int_copy(BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_COPY);

    // NOTE: For arbitrary sizes, we'd need a realloc here
    if (a->alloc_size < b->size)
        FATAL("Cannot copy larger BigInt into smaller one!\n");

    a->sign     = b->sign;
    a->overflow = b->overflow;
    a->size     = b->size;

    memcpy((void *) a->chunks, (void *) b->chunks, b->size * sizeof(dbl_chunk_size_t));
    return a;
}

/**
 * \brief Duplicate a BigInt
 */
BigInt *big_int_duplicate(BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DUPLICATE);

    BigInt *b;

    b = big_int_alloc(a->alloc_size);

    return big_int_copy(b, a);
}


/**
 * \brief Print a BigInt to stdout
 */
void big_int_print(BigInt *a)
{
    printf("%s0x%08" PRIx64, (a->sign == 1) ? "-" : "", a->chunks[a->size-1]);
    if (a->size >= 2) {
        for (int64_t i = a->size-2; i >= 0; --i) {
            printf(" %08" PRIx64, a->chunks[i]);
        }
    }
}


/**
 * \brief Calculate r = -a
 * \param r Result value, see big_int_get_res.
 */
BigInt *big_int_neg(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_NEG);

    r = big_int_get_res(r, a);
    r->sign = !r->sign;

    return r;
}


/**
 * \brief Calculate r = |a|
 * \param r Result value, see big_int_get_res.
 */
BigInt *big_int_abs(BigInt *r, BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ABS);

    r = big_int_get_res(r, a);
    r->sign = 0;

    return r;
}


/**
 * \brief Calculate r = a + b
 * \param r Result value, see big_int_get_res.
 */
BigInt *big_int_add(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ADD);

    BigInt *aa, *bb, *neg;
    uint8_t carry;
    int64_t i;
    dbl_chunk_size_t sum;
    uint64_t r_size;

    // Use subtractions when adequate
    if (a->sign != b->sign) {
        if (a->sign == 0) {
            neg = big_int_neg(NULL, b);
            r = big_int_sub(r, a, neg);
        }
        else {
            neg = big_int_neg(NULL, a);
            r = big_int_sub(r, b, neg);
        }
        big_int_destroy(neg);
        return r;
    }

    // Note aliasing is not an issue, since we only read potential shared
    // values _before_ we overwrite them.

    // Assertion: a->sign == b->sign

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

    // NOTE: for non-fixed BigInts we would need to make sure that r is at least
    // of size aa->size + 1
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    // First, add chunks where both have entries
    carry = 0;
    r_size = 0;
    for (i = 0; i < bb->size; ++i) {
        sum = aa->chunks[i] + bb->chunks[i] + carry;
        r->chunks[i] = sum % BIGINT_RADIX;
        carry = sum / BIGINT_RADIX;

        if (r->chunks[i] != 0)
            r_size = i;
    }

    // Second, finish possible remaining chunks of larger integer
    for (; i < aa->size; ++i) {
        sum = aa->chunks[i] + carry;
        r->chunks[i] = sum % BIGINT_RADIX;
        carry = sum / BIGINT_RADIX;

        if (r->chunks[i] != 0)
            r_size = i;
    }

    // Since both BigInts have the same sign, the result has the same sign too
    // except if there was an overflow that flipped it.

    r->sign = aa->sign;
    if (carry) {
        if (i < r->alloc_size) {
            r->chunks[i] = 1;
            r_size = i;
        }
        else {
            WARNING("Addition overflow detected. Currently operations are limited to 256 bits.\n");
            r->overflow = 1;
            r->sign = !r->sign;
        }
    }
    r->size = r_size + 1;

    return r;
}


/**
 * \brief Calculate r = a - b
 */
BigInt *big_int_sub(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SUB);

    BigInt *b_neg, *a_abs, *b_abs, *aa_abs, *bb_abs;
    int borrow;
    int64_t i;
    dbl_chunk_size_t diff;
    uint8_t do_sign_switch;
    uint64_t r_size;

    // Use addition operations depending on signs
    if (a->sign != b->sign) {
        b_neg = big_int_neg(NULL, b);
        r = big_int_add(r, a, b_neg);
        big_int_destroy(b_neg);
        return r;
    }

    // If a, b < 0; calculate -(|a| - |b|) instead
    if (a->sign == 1) {
        a_abs = big_int_abs(NULL, a);
        b_abs = big_int_abs(NULL, b);
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

    // NOTE: for arbitrary size BigInts, r has to be at least of size aa_abs->size
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    // Note an underflow sets the 1 bit at position MSB+1 of the chunk:
    // 0x0000000000000000 - 1 = 0xffffffff00000000
    //  | extra | chunk |        | extra | chunk |
    borrow = 0;
    r_size = 0;
    for (i = 0; i < bb_abs->size; ++i) {
        diff = aa_abs->chunks[i] - bb_abs->chunks[i] - borrow;
        r->chunks[i] = diff % BIGINT_RADIX;
        borrow = (diff / BIGINT_RADIX) & 1;

        if (r->chunks[i] != 0)
            r_size = i;
    }

    for (; i < aa_abs->size; ++i) {
        diff = aa_abs->chunks[i] - borrow;
        r->chunks[i] = diff % BIGINT_RADIX;
        borrow = (diff / BIGINT_RADIX) & 1;

        if (r->chunks[i] != 0)
            r_size = i;
    }

    // Assertion: borrow = 0, because aa_abs >= bb_abs

    // There is never an overflow, because we subtract two numbers with diff. signs
    r->overflow = 0;

    // The sign is zero, because |a| >= |b| and we calculate |a| - |b|
    r->sign = 0;
    r->size = r_size + 1;

    if (a->sign == 1) {
        big_int_destroy(a_abs);
        big_int_destroy(b_abs);
    }

    return (do_sign_switch) ? big_int_neg(r, r) : r;
}


/**
 * \brief Calculate a * b
 */
BigInt *big_int_mul(BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MUL);

    int64_t i, j;
    dbl_chunk_size_t carry;
    BigInt *r_loc;

    // NOTE: for arbitrary sized BigInts, this would define the alloc/realloc
    // size and not throw an error.
    if (a->size + b->size > BIGINT_FIXED_SIZE)
        FATAL("Product requires %" PRIu64 " > %" PRIu64 " bytes to be stored!\n",
            (uint64_t) a->size + b->size, BIGINT_FIXED_SIZE);
    if (r && r->alloc_size < a->size + b->size)
        FATAL("Result array for product is too small, requires %" PRIu64 " > %" PRIu64 " bytes\n",
            (uint64_t) a->size + b->size, BIGINT_FIXED_SIZE);

    // For MUL, we we have aliasing and a separate BigInt for r is necessary

    // zero out r_loc
    r_loc = big_int_calloc(BIGINT_FIXED_SIZE);

    r_loc->size = a->size + b->size;
    r_loc->sign = a->sign ^ b->sign;

    for (i = 0; i < b->size; ++i) {
        // shortcut for zero chunk
        if (b->chunks[i] == 0)
            r_loc->chunks[i + a->size] = 0;
        else {
            // Multiply and add chunks
            carry = 0;
            for (j = 0; j < a->size; ++j) {
                carry += a->chunks[j] * b->chunks[i] + r_loc->chunks[i + j];
                r_loc->chunks[i + j] = carry % BIGINT_RADIX;
                carry /= BIGINT_RADIX;
            }
            r_loc->chunks[i + a->size] = carry;
        }
    }
    big_int_prune_leading_zeros(r_loc, r_loc);

    if (r) {
        big_int_copy(r, r_loc);
        big_int_destroy(r_loc);
        return r;
    }

    return r_loc;
}


/**
 * \brief Calculate quotient q and remainder r, such that: a = q * b + r
 *        We round numbers towards 0, e.g., -13/8 = (-1, -5)
 * \param r if r is not NULL, it will be set to the remainder.
 * \returns pointer to q
 */
BigInt *big_int_div_rem(BigInt *q, BigInt *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DIV_REM);

    dbl_chunk_size_t a_tmp, b_tmp, tmp, q_c, r_c;
    uint8_t q_sign;
    uint64_t factor;
    int64_t q_idx, a_idx, i;
    BigInt *a_abs, *b_abs, *a_loc, *b_loc, *a_part,
           *q_c_bigint, *qb, *radix_pow;
    BigInt *r_loc = NULL;

    if (big_int_is_zero(b))
        FATAL("Division by zero!\n");

    // DEBUG_BIGINT(a, "div_rem; a = ");
    // DEBUG_BIGINT(b, "div_rem; b = ");

    /*
     * Special cases
     */
    a_abs = big_int_abs(NULL, a);
    b_abs = big_int_abs(NULL, b);

    // zero dividend
    if (big_int_is_zero(a)) {
        q = big_int_create_from_chunk(q, 0, 0);
        r_loc = big_int_create_from_chunk(NULL, 0, 0);
    }
    // divisor larger than dividend
    else if (big_int_compare(b_abs, a_abs) == 1) {
        q = big_int_create_from_chunk(q, 0, 0);

        // Save unsigned rest, account for signs later
        r_loc = big_int_duplicate(a);
        big_int_abs(r_loc, r_loc);
    }
    // Simple case for small BigInts, just use normal C division (but round
    // towards -inf at the end)
    else if (a->size == 1) {
        q = big_int_create_from_chunk(q, a->chunks[0] / b->chunks[0], 0);
        q->sign = a->sign ^ b->sign;
        r_loc = big_int_create_from_chunk(r_loc, a->chunks[0] % b->chunks[0], 0);
    }
    // Since we do operations on double chunks, we can also do the shortcut for length 2
    else if (a->size == 2) {
        a_tmp = a->chunks[1] * BIGINT_RADIX + a->chunks[0];
        b_tmp = b->chunks[0];
        if (b->size == 2)
            b_tmp += b->chunks[1] * BIGINT_RADIX;
        q_sign = a->sign ^ b->sign;

        q = big_int_create_from_dbl_chunk(q, a_tmp / b_tmp, q_sign);
        r_loc = big_int_create_from_dbl_chunk(r_loc, a_tmp % b_tmp, 0);
    }
    else {
        // Below: Division for a->size > 2

        // NOTE: for arbitrary sized BigInts, this size would be a->size - b->size + 1
        if (!q)
            q = big_int_alloc(BIGINT_FIXED_SIZE);

        q->sign = a->sign ^ b->sign;
        q->size = a->size - b->size + 1;

        // Preserve values of a, b
        a_loc = big_int_duplicate(a);
        b_loc = big_int_duplicate(b);

        // Operate on absolute values, account for signs at the end
        big_int_abs(a_loc, a_loc);
        big_int_abs(b_loc, b_loc);

        // Normalize: find factor such that b->chunks[b->size - 1] >= BIGINT_RADIX/2
        factor = 0;
        tmp = b_loc->chunks[b->size - 1];
        while (2 * tmp < BIGINT_RADIX) {
            ++factor;
            tmp <<= 1;
        }

        if (factor > 0) {
            big_int_sll_small(a_loc, a_loc, factor);
            big_int_sll_small(b_loc, b_loc, factor);
        }

        if (a_loc->size == a->size) {
            // Special case where we actually have a zero in the MSB, just to make
            // subsequent ops easier
            a_loc->chunks[a->size] = 0;
            a_loc->size += 1;
        }

        // Prepare for intermediate BigInts
        // NOTE: for arbitrary sized BigInts, this really only needs to be of size b->size
        a_part = big_int_alloc(BIGINT_FIXED_SIZE);
        // NOTE: for arbitrary sized BigInts, this really only needs to be of size 1
        q_c_bigint = big_int_alloc(BIGINT_FIXED_SIZE);
        // NOTE: for arbitrary sized BigInts, this really only needs to be of size b->size+1
        qb = big_int_alloc(BIGINT_FIXED_SIZE);
        // NOTE: for arbitrary sized BigInts, this really only needs to be of size b->size+1
        radix_pow = big_int_calloc(BIGINT_FIXED_SIZE);
        radix_pow->chunks[b->size] = 1;
        radix_pow->size = b->size + 1;

        // Calculate quotient digit by digit
        for (q_idx = q->size - 1; q_idx >= 0; --q_idx) {
            // Calculate quotient and remainder of
            // a->chunks[q_idx : q_idx + b->size] / b_loc

            a_idx = q_idx + b->size;

            q_c = a_loc->chunks[a_idx] * BIGINT_RADIX + a_loc->chunks[a_idx - 1];
            r_c = q_c % b_loc->chunks[b->size - 1];
            q_c /= b_loc->chunks[b->size - 1];

            do {
                if (q_c >= BIGINT_RADIX
                    || (b->size > 1
                        && q_c * b_loc->chunks[b->size - 2] >
                           BIGINT_RADIX * r_c + a_loc->chunks[a_idx - 2]))
                {
                    --q_c;
                    r_c += b_loc->chunks[b_loc->size - 1];
                }
                else {
                    break;
                }
            } while (r_c < BIGINT_RADIX);

            // Multiply and subtract
            // TODO: this entire part can surely be done more efficiently
            for (i = 0; i <= b_loc->size; ++i) {
                if (q_idx + i >= a_loc->size)
                    break;
                a_part->chunks[i] = a_loc->chunks[q_idx + i];
            }
            a_part->size = i;
            a_part->sign = 0;

            big_int_create_from_chunk(q_c_bigint, q_c, 0);
            big_int_sub(qb, a_part, big_int_mul(qb, q_c_bigint, b_loc));

            // TODO: test this case specifically, apparently this is a very unlikely
            // case (cf. step D6 in D. Knuth's book section 4.3)
            if (qb->sign == 1) {
                // WARNING("Rare case in div_rem triggered\n");
                big_int_add(qb, qb, radix_pow);

                --q_c;
                // NOTE: we intentionally ignore the overflow in a_part, it cancels
                // out with the borrow that we ignored from the previous add.
                big_int_add(qb, qb, b_loc);

                for (i = 0; i <= b->size; ++i) {
                    if (q_idx + i >= a_loc->size)
                        break;
                    a_loc->chunks[q_idx + i] = qb->chunks[i];
                }
            }
            else {
                for (i = 0; i <= b->size; ++i) {
                    if (q_idx + i >= a_loc->size)
                        break;
                    a_loc->chunks[q_idx + i] = qb->chunks[i];
                }
            }

            q->chunks[q_idx] = q_c;
        }

        // Unnormalize
        a_loc->size = b->size;
        r_loc = big_int_duplicate(a_loc);
        big_int_srl_small(r_loc, r_loc, factor);
        big_int_prune_leading_zeros(r_loc, r_loc);

        big_int_prune_leading_zeros(q, q);

        // Cleanup intermediate BigInts specific to this case
        big_int_destroy(a_part);
        big_int_destroy(a_loc);
        big_int_destroy(b_loc);
        big_int_destroy(q_c_bigint);
        big_int_destroy(radix_pow);
        big_int_destroy(qb);
    }

    // Round towards -inf if either operand is negative
    if (a->sign ^ b->sign && !big_int_is_zero(r_loc)) {
        big_int_sub(q, q, big_int_one);

        b_loc = big_int_abs(NULL, b);
        big_int_sub(r_loc, b_loc, r_loc);
        big_int_destroy(b_loc);
    }
    // If b is negative, flip the sign of the remainder to maintain the
    // invariant q * b + r = a
    if (b->sign)
        big_int_neg(r_loc, r_loc);

    if (r)
        big_int_copy(r, r_loc);

    // Cleanup intermediate BigInts
    big_int_destroy(a_abs);
    big_int_destroy(b_abs);
    big_int_destroy(r_loc);

    return q;
}


/**
 * \brief Calculate r = a // b (integer division)
 */
BigInt *big_int_div(BigInt *q, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DIV);

    return big_int_div_rem(q, NULL, a, b);
}


/**
 * \brief Calculate r = a << shift for "small" shifts (not BigInt shifts)
 */
BigInt *big_int_sll_small(BigInt *r, BigInt *a, uint64_t shift)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SLL_SMALL);

    BigInt *r_loc;

    dbl_chunk_size_t carry;
    uint64_t r_size;
    int64_t i, r_idx;

    r_size = a->size + shift / BIGINT_CHUNK_BIT_SIZE;
    // NOTE: for arbitrary sized BigInts, this would cause define the new/realloc
    // size for r!
    if (r_size > BIGINT_FIXED_SIZE)
        FATAL("Shift creates too large BigInt (%" PRIu64 " chunks)!\n", r_size);

    r_loc = big_int_alloc(BIGINT_FIXED_SIZE);

    // Set the lowest chunks to zero
    for (r_idx = 0; r_idx < (int64_t) (shift / BIGINT_CHUNK_BIT_SIZE); ++r_idx) {
        r_loc->chunks[r_idx] = 0;
    }

    // Shift the other chunks by the chunk internal shift
    shift %= BIGINT_CHUNK_BIT_SIZE;
    carry = 0;
    for (i = 0; i < a->size; ++i) {
        carry = (a->chunks[i] << shift) | carry;
        r_loc->chunks[r_idx] = carry % BIGINT_RADIX;
        carry /= BIGINT_RADIX;

        ++r_idx;
    }

    r_loc->overflow = 0;
    r_loc->sign = a->sign;

    // Add the last block if there is a carry from the MSB block
    if (carry) {
        if (r_idx < r->alloc_size) {
            r_loc->chunks[r_idx] = carry;
            ++r_idx;
        }
        else {
            WARNING("sll caused overflow!\n");
            r_loc->overflow = 1;
        }
    }

    r_loc->size = r_idx;

    if (r) {
        big_int_copy(r, r_loc);
        big_int_destroy(r_loc);
        return r;
    }

    return r_loc;
}


/**
 * \brief Calculate r = a >> shift for "small" shifts (not BigInt shifts)
 */
BigInt *big_int_srl_small(BigInt *r, BigInt *a, uint64_t shift)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SRL_SMALL);

    BigInt *last_bit_bigint;
    dbl_chunk_size_t carry, last_bit;
    int64_t i, a_idx;

    // NOTE: for arbitrary sized BigInts, r would only need to be of size
    // a->size - shift / BIGINT_CHUNK_BIT_SIZE
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    // Special case where we shift away all bits
    if (shift / BIGINT_CHUNK_BIT_SIZE >= a->size) {
        // Special case for rounding towards -inf
        if (a->sign && (shift - 1) / BIGINT_CHUNK_BIT_SIZE < a->size) {
            big_int_create_from_chunk(r,
                a->chunks[a->size - 1] >> (((shift - 1) % BIGINT_CHUNK_BIT_SIZE)),
                0);
            r->sign = 1;
        }
        else {
            big_int_create_from_chunk(r, 0, 0);
        }
        return r;
    }

    r->size = a->size - shift / BIGINT_CHUNK_BIT_SIZE;

    // Take the chunks of a that are not all completely shifted away
    a_idx = (int64_t) (shift / BIGINT_CHUNK_BIT_SIZE);
    shift %= BIGINT_CHUNK_BIT_SIZE;

    // Store the last bit that is shifted away to implement rounding towards -inf
    last_bit = (a->chunks[a_idx] >> (shift - 1)) & 1;

    // Go upwards to avoid aliasing, since we always write r[i] and read
    // a[a_idx+1], where i < a_idx + 1.
    for (i = 0; i < r->size-1; ++i) {
        carry = (a->chunks[a_idx + 1] << (BIGINT_CHUNK_BIT_SIZE - shift)) % BIGINT_RADIX;
        r->chunks[i] = carry | (a->chunks[a_idx] >> shift);
        ++a_idx;
    }
    // Write MSB entry
    r->chunks[r->size - 1] = a->chunks[a_idx] >> shift;

    // Add last bit (round towards -inf)
    if (a->sign && last_bit) {
        last_bit_bigint = big_int_create_from_chunk(NULL, last_bit, 0);
        big_int_sub(r, r, last_bit_bigint);
        big_int_destroy(last_bit_bigint);
    }

    r->overflow = 0;
    r->sign = a->sign;

    return big_int_prune_leading_zeros(r, r);
}


/**
 * \brief Calculate r := a mod q
 */
BigInt *big_int_mod(BigInt *r, BigInt *a, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MOD);

    BigInt *tmp;

    // NOTE: for arbitrary sized BigInts, r would only need to be of size q->size
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    // TODO: this can be optimized for special cases. We need to check if
    // q (for the mapping) is close to a power of two. In that case, we might
    // be able to avoid using division here!
    tmp = big_int_div_rem(NULL, r, a, q);

    big_int_destroy(tmp);

    return r;
}

// TODO: Evaluate if it's worth it to add specialized implementations for all
//       those *_mod operations.
// TODO: Evaluate if it's worth it to take the arguments a, b mod q before
//       doing the operation. (For now, probably not as this uses division,
//       but might be interesting later, if we can make mod faster).

/**
 * \brief Calculate r := (a + b) mod q
 */
BigInt *big_int_add_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_ADD_MOD);

    BigInt *r_loc;

    r_loc = big_int_add(NULL, a, b);
    r = big_int_mod(r, r_loc, q);

    big_int_destroy(r_loc);

    return r;
}


/**
 * \brief Calculate r := (a - b) mod q
 */
BigInt *big_int_sub_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_SUB_MOD);

    BigInt *r_loc;

    r_loc = big_int_sub(NULL, a, b);
    big_int_mod(r, r_loc, q);

    big_int_destroy(r_loc);

    return r;
}


/**
 * \brief Calculate r := (a * b) mod q
 */
BigInt *big_int_mul_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_MUL_MOD);

    BigInt *r_loc;

    r_loc = big_int_mul(NULL, a, b);
    r = big_int_mod(r, r_loc, q);

    big_int_destroy(r_loc);

    return r;
}


/**
 * \brief Calculate r := (a * b^-1) mod q
 */
BigInt *big_int_div_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_DIV_MOD);

    BigInt *r_loc;

    // Write to local copy to be save against pointer reuse
    r_loc = big_int_inv(NULL, b, q);
    r = big_int_mul_mod(r, a, r_loc, q);

    big_int_destroy(r_loc);

    return r;
}


/**
 * \brief Calculate r := a^-1 mod q (the inverse of a)
 */
BigInt *big_int_inv(BigInt *r, BigInt *a, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_INV);

    EgcdResult res;

    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    big_int_egcd(&res, a, q);
    if (big_int_compare(res.g, big_int_one) != 0) {
        big_int_destroy(res.g);
        big_int_destroy(res.x);
        big_int_destroy(res.y);
        FATAL("Non-invertible number given as argument to big_int_inv!\n");
        return NULL;
    }

    // set r = x mod q
    big_int_mod(r, res.x, q);

    big_int_destroy(res.g);
    big_int_destroy(res.x);
    big_int_destroy(res.y);

    return r;
}


/**
 * \brief Calculate r := (b^e) mod q
 */
BigInt *big_int_pow(BigInt *r, BigInt *b, BigInt *e, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_POW);

    BigInt *b_loc, *e_loc, *r_loc;

    // NOTE: for arbitrary sized BigInts, we'd need to figure out some good size
    //       to allocate for the first r. For now, this implicitly use the fixed size.
    r_loc = big_int_create_from_chunk(NULL, 1, 0);
    e_loc = big_int_duplicate(e);
    b_loc = big_int_duplicate(b);

    while (big_int_compare(e_loc, big_int_zero) > 0) {
        // If power is odd
        if (big_int_is_odd(e_loc))
            big_int_mul_mod(r_loc, r_loc, b_loc, q);

        big_int_srl_small(e_loc, e_loc, 1);
        big_int_mul_mod(b_loc, b_loc, b_loc, q);
    }

    big_int_destroy(e_loc);
    big_int_destroy(b_loc);

    if (r) {
        big_int_copy(r, r_loc);
        big_int_destroy(r_loc);
        return r;
    }
    else {
        return r_loc;
    }
}


/**
 * \brief Returns true if the BigInt is zero
 */
int8_t big_int_is_zero(BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_IS_ZERO);

    return a->size == 1 && a->chunks[0] == 0;
}


/**
 * \brief Returns true if the BigInt is an odd number
 */
int8_t big_int_is_odd(BigInt *a)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_IS_ODD);

    return (a->size > 0) && (a->chunks[0] & 1);
}


/**
 * \brief Calculate a == b.
 * \returns r, where a==b: r=0, a<b: r<0, a>b: r>0
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
    else {
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
 * \returns EgcdResult (x, y, g), where x * a + y * b = g
 *          and g is the GCD.
 */
EgcdResult *big_int_egcd(EgcdResult *r, BigInt *a, BigInt *b)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_EGCD);

    BigInt *q, *a_loc, *b_loc, *x0, *x1, *y0, *y1, *tmp;

    if (big_int_is_zero(a) && big_int_is_zero(b)) {
        r->x = big_int_create_from_chunk(NULL, 0, 0);
        r->y = big_int_create_from_chunk(NULL, 0, 0);
        r->g = big_int_create_from_chunk(NULL, 0, 0);
        return r;
    }

    q = big_int_alloc(BIGINT_FIXED_SIZE);
    tmp = big_int_alloc(BIGINT_FIXED_SIZE);
    a_loc = big_int_duplicate(a);
    b_loc = big_int_duplicate(b);

    // Take the GCD of positive numbers, then just flig the signs of x, y at the end
    big_int_abs(a_loc, a_loc);
    big_int_abs(b_loc, b_loc);

    x0 = big_int_create_from_chunk(NULL, 0, 0);
    x1 = big_int_create_from_chunk(NULL, 1, 0);
    y0 = big_int_create_from_chunk(NULL, 1, 0);
    y1 = big_int_create_from_chunk(NULL, 0, 0);

    while (big_int_compare(a_loc, big_int_zero) != 0)
    {
        big_int_div_rem(q, tmp, b_loc, a_loc);
        big_int_copy(b_loc, a_loc);
        big_int_copy(a_loc, tmp);

        big_int_copy(tmp, y1);
        big_int_sub(y1, y0, big_int_mul(y1, y1, q));
        big_int_copy(y0, tmp);

        big_int_copy(tmp, x1);
        big_int_sub(x1, x0, big_int_mul(x1, x1, q));
        big_int_copy(x0, tmp);

    }

    // Account for signs of a and b
    x0->sign ^= a->sign;
    y0->sign ^= b->sign;

    r->g = b_loc;
    r->x = x0;
    r->y = y0;

    big_int_destroy(tmp);
    big_int_destroy(q);
    big_int_destroy(x1);
    big_int_destroy(y1);
    big_int_destroy(a_loc);

    return r;
}

/**
 * \brief Free BigInts in eGCD result
 */
void big_int_free_egcd_result(EgcdResult *r)
{
    free(r->g);
    free(r->x);
    free(r->y);
}

/**
 * \brief Calculate the Chi function chi(t) = t**((q-1)/2) mod q
 * \returns 0 if t = 0, 1 if t is a non-zero square, -1 otherwise
 */
BigInt *big_int_chi(BigInt *r, BigInt *t, BigInt *q)
{
    ADD_STAT_COLLECTION(BIGINT_TYPE_BIG_INT_CHI);

    BigInt *e;

    // Check assumption that q is odd (otherwise our chi function might need to
    // be changed)
    if (!big_int_is_odd(q))
        FATAL("Chi function implementation only supports odd q (actually prime q)!");

    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    if (big_int_compare(t, big_int_zero) == 0)
        return big_int_create_from_chunk(r, 0, 0);

    e = big_int_duplicate(q);
    big_int_srl_small(e, big_int_sub(e, e, big_int_one), 1);

    big_int_pow(r, t, e, q);

    big_int_destroy(e);

    if (!big_int_compare(r, big_int_zero) || !big_int_compare(r, big_int_one))
        return r;
    return big_int_create_from_chunk(r, 1, 1); // r = -1

}


// This function has to be at the end of this file for the type auto-generation
// to work.
void reset_stats()
{
    for (int64_t i = 0; i < BIGINT_TYPE_LAST; ++i)
        big_int_stats[i] = 0;
}

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
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

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
BigInt *big_int_alloc(uint64_t size);
BigInt *big_int_calloc(uint64_t size);
BigInt *big_int_get_res(BigInt *r, BigInt *a);
BigInt *big_int_create_from_dbl_chunk(BigInt *r, dbl_chunk_size_t chunk,
    uint8_t sign);

/**
 * \brief Allocate a BigInt of the given size
 */
BigInt *big_int_alloc(uint64_t size)
{
    BigInt *a;

    a = (BigInt *) malloc(sizeof(BigInt));
    if (!a)
        FATAL("Failed to malloc BigInt.\n");

    a->chunks = (dbl_chunk_size_t *) malloc(size * sizeof(dbl_chunk_size_t));
    if (!a->chunks) {
        free(a);
        FATAL("Failed to malloc %llu chunks for malloc.\n", size);
    }
    a->alloc_size = size;

    return a;
}

/**
 * \brief Allocate a BigInt of the given size
 */
BigInt *big_int_calloc(uint64_t size)
{
    BigInt *a;

    a = (BigInt *) calloc(1, sizeof(BigInt));
    if (!a)
        FATAL("Failed to calloc BigInt.\n");

    a->chunks = (dbl_chunk_size_t *) calloc(size, sizeof(dbl_chunk_size_t));
    if (!a->chunks) {
        free(a);
        FATAL("Failed to calloc %llu chunks for malloc.\n", size);
    }
    a->alloc_size = size;

    return a;
}


/**
 * \brief if r = NULL:  allocate new BigInt, return pointer to new BigInt
 *        otherwise:    use the given BigInt, return pointer to r. Note that if
 *                      r = a, then this is an in-place modification.
 */
BigInt *big_int_get_res(BigInt *r, BigInt *a) {
    return (!r) ? big_int_duplicate(a) : r;
}


/**
 * \brief Create BigInt from 64-bit integer
 *        Precondition: x fits into chunk_size_t
 */
BigInt *big_int_create(BigInt *r, int64_t x)
{
    if (x > BIGINT_RADIX_SIGNED || x == INT64_MIN || x < -BIGINT_RADIX_SIGNED)
        FATAL("Integer %lli does not fit into a single chunk of %lu bytes\n",
            x, sizeof(chunk_size_t));

    // NOTE: Currently, all BigInts have size BIGINT_FIXED_SIZE
    if (!r)
        r = big_int_alloc(BIGINT_FIXED_SIZE);

    r->chunks[0] = (dbl_chunk_size_t) CHUNK_ABS(x);
    r->sign      = x < 0;
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
        FATAL("Integer %s is larger than %llu bytes and currently not supported.",
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
 * \brief Free a BigInt
 */
void big_int_destroy(BigInt *a)
{
    if (a->alloc_size > 0)
        free(a->chunks);
    free(a);
}


/**
 * \brief Copy BigInt b to BigInt a, i.e., a := b
 */
void big_int_copy(BigInt *a, BigInt *b)
{
    // NOTE: For arbitrary sizes, we'd need a realloc here
    if (a->alloc_size < b->size)
        FATAL("Cannot copy larger BigInt into smaller one!\n");

    a->sign     = b->sign;
    a->overflow = b->overflow;
    a->size     = b->size;

    memcpy((void *) a->chunks, (void *) b->chunks, b->size * sizeof(dbl_chunk_size_t));
}

/**
 * \brief Duplicate a BigInt
 */
BigInt *big_int_duplicate(BigInt *a)
{
    BigInt *b;

    b = big_int_alloc(a->size);
    big_int_copy(b, a);

    return b;
}


/**
 * \brief Print a BigInt to stdout
 */
void big_int_print(BigInt *a)
{
    printf("%s0x%llx", (a->sign == 1) ? "-" : "", a->chunks[a->size-1]);
    for (int64_t i = a->size-2; i >= 0; --i) {
        printf("%08llx", a->chunks[i]);
    }
}


/**
 * \brief Calculate r = -a
 * \param r Result value, see big_int_get_res.
 */
BigInt *big_int_neg(BigInt *r, BigInt *a)
{
    r = big_int_get_res(r, a);
    r->sign = !a->sign;

    return r;
}


/**
 * \brief Calculate r = |a|
 * \param r Result value, see big_int_get_res.
 */
BigInt *big_int_abs(BigInt *r, BigInt *a)
{
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
    r = big_int_get_res(r, aa);

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
            r_size++;
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
    r = big_int_get_res(r, aa_abs);

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
    r->overflow = do_sign_switch;
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
    int64_t i, j;
    dbl_chunk_size_t carry;
    BigInt *r_loc;

    // NOTE: for arbitrary sized BigInts, this would define the alloc/realloc
    // size and not throw an error.
    if (a->size + b->size > BIGINT_FIXED_SIZE)
        FATAL("Product requires %llu > %llu bytes to be stored!\n",
            a->size + b->size, BIGINT_FIXED_SIZE);
    if (r && r->alloc_size < a->size + b->size)
        FATAL("Result array for product is too small, requires %llu > %llu bytes\n",
            a->size + b->size, BIGINT_FIXED_SIZE);

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

    // Find actual size of r (at least 1)
    for (i = r_loc->size - 1; i > 0; --i) {
        if (r_loc->chunks[i])
            break;
        r_loc->size--;
    }

    if (r) {
        big_int_copy(r, r_loc);
        big_int_destroy(r_loc);
    }

    return r;
}


/**
 * \brief Calculate quotient q and remainder r, such that: a = q * b + r
 * \param r if r is not NULL, it will be set to the remainder.
 * \returns pointer to q
 */
BigInt *big_int_div_rem(BigInt *q, BigInt *r, BigInt *a, BigInt *b)
{
    dbl_chunk_size_t a_tmp, b_tmp;
    BigInt *factor;

    // NOTE: for arbitrary sized BigInts, this size would be a->size - b->size + 1
    if (!q)
        q = big_int_alloc(BIGINT_FIXED_SIZE);

    // Special cases that return zero: divisor larger than dividend, zero dividend
    if (b->size > a->size || big_int_is_zero(a))
        return big_int_create(r, 0);

    if (big_int_is_zero(b))
        FATAL("Division by zero!");

    q->sign = a->sign ^ b->sign;

    // Simple case for small BigInts, just use normal C division
    if (a->size <= 2) {
        a_tmp = a->chunks[1] * BIGINT_RADIX + a->chunks[0];
        b_tmp = b->chunks[1] * BIGINT_RADIX + b->chunks[0];

        big_int_create_from_dbl_chunk(q, a_tmp / b_tmp, q->sign);
        if (r)
            big_int_create_from_dbl_chunk(r, a_tmp % b_tmp, q->sign);
        return q;
    }

    // Division for a->size > 2
    // Normalize
    // TODO: choose power of two here?
    factor = big_int_create(NULL, BIGINT_RADIX / (b->chunks[b->size - 1] + 1));
    big_int_mul(a, a, factor);

    // TODO: finish this implementation

    return r;
}

/**
 * \brief Calculate r = a // b (integer division)
 */
BigInt *big_int_div(BigInt *q, BigInt *a, BigInt *b)
{
    return big_int_div_rem(q, NULL, a, b);
}

// /**
//  * \brief Calculate base^exponent mod q for BigInts
//  */
// BigInt big_int_pow(BigInt base, BigInt exponent, BigInt q)
// {
//     BigInt result = create_big_int(1);
//     BigInt p = exponent;
//     BigInt b = base;
//
//     while (big_int_compare(p, create_big_int(0)) > 0)
//     {
//         // If power is odd
//         if (big_int_compare(big_int_mod(p, create_big_int(2)), create_big_int(1)) == 0)
//         {
//             result = big_int_mod(big_int_mul(result, b), q);
//         }
//         p = big_int_div(p, create_big_int(2));
//         p = big_int_mod(big_int_mul(b, b), q);
//     }
//     return result;
// }
//
// /**
//  * \brief Calculate (a // b) mod q (integer division mod q)
//  */
// // TODO: uncomment when big_int_mul_mod exists
// // BigInt big_int_div_mod(BigInt a, BigInt b, BigInt q)
// // {
// //     return big_int_mul_mod(a, big_int_inverse(b, q), q);
// // }
//
//
// /**
//  * \brief Calculate the inverse of a, namely a^-1 mod q
//  */
// // TODO: change for 256 bits
// BigInt big_int_inverse(BigInt a, BigInt q)
// {
//     // Modular inversion computation
//     if (big_int_compare(a, create_big_int(0)) < 0)
//     {
//         BigInt r0 = big_int_negate(a);
//         r0 = big_int_inverse(r0, q);
//         return big_int_sub(q, r0);
//     }
//
//     egcd_result res = egcd(a, q);
//     if (big_int_compare(res.g, create_big_int(1)) != 0)
//         assert(!"Non-invertible number given as argument to big_int_inverse.");
//     else
//         return big_int_mod(res.x, q);
// }
//
//
// /**
//  * \brief Calculate a mod q
//  */
// // TODO: change for 256 bits
// BigInt big_int_mod(BigInt a, BigInt q)
// {
//     BigInt r = {a.x % q.x};
//     return r;
// }


/**
 * \brief Returns true if the BigInt is zero
 */
int8_t big_int_is_zero(BigInt *a)
{
    return a->size == 1 && a->chunks[0] == 0;
}


/**
 * \brief Calculate a == b.
 * \returns r, where a==b: r=0, a<b: r<0, a>b: r>0
 */
int8_t big_int_compare(BigInt *a, BigInt *b)
{
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


// /**
//  * \brief Calculate the greatest common divisor using the extended Euclidean
//  *        algorithm.
//  * \returns egcd_result (x, y, g), where x * a + y * b = g
//  *          and g is the GCD.
//  */
// // TODO: change for 256 bits
// // TODO: Change to iterative instead of recursive variant
// egcd_result egcd(BigInt a, BigInt b) // Euclidean algorithm for gcd computation
// {
//     if (big_int_compare(a, create_big_int(0)) == 0)
//     {
//         egcd_result r = {b, create_big_int(0), create_big_int(1)};
//         return r;
//     }
//     else
//     {
//         egcd_result res = egcd(big_int_mod(b, a), a);
//         BigInt r0 = big_int_div(b, a);
//         r0 = big_int_mul(r0, res.y);
//         egcd_result r = {res.g, big_int_sub(res.x, r0), res.y};
//         return r;
//     }
// }
//
//
// /**
//  * \brief Calculate the Chi function chi(t) = t**((q-1)/2)
//  * \returns 0 if t = 0, 1 if t is a non-zero square, -1 otherwise (t is not a
//  *          square)
//  */
// BigInt chi(BigInt t, BigInt q)
// {
//     BigInt r0 = big_int_inverse(create_big_int(2), q);
//     BigInt r1 = big_int_sub(q, create_big_int(1));
//     r0 = big_int_mul(r1, r0);
//     r0 = big_int_pow(t, r0, q);
//     return r0;
// }

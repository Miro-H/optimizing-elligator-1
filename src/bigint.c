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
 * Function prototypes
 */
BigInt *big_int_alloc(uint64_t size);



/**
 * \brief Allocate a BigInt of the given size
 */
BigInt *big_int_alloc(uint64_t size)
{
    BigInt *a;

    a = (BigInt *) malloc(sizeof(BigInt));
    if (!a)
        FATAL("Failed to malloc BigInt.\n");

    a->chunks = (uint64_t *) malloc(size * sizeof(uint64_t));
    if (!a->chunks) {
        free(a);
        FATAL("Failed to malloc %lu chunks for malloc.\n", size);
    }
    a->alloc_size = size;

    return a;
}


/**
 * \brief Create BigInt from 64-bit integer
 *        Precondition x >= -INT64_MIN
 */
BigInt *big_int_create(int64_t x)
{
    BigInt *a;

    if (x < INT64_MIN)
        FATAL("Integer %li has no unsigned equivalent and is thus not accepted as input!\n", x);

    // NOTE: Currently, all BigInts have size BIGINT_FIXED_SIZE
    a = big_int_alloc(BIGINT_FIXED_SIZE);

    a->chunks[0] = llabs(x);
    a->sign      = x < 0;
    a->overflow  = 0;
    a->size      = 1;

    return a;
}


/**
 * \brief Create BigInt from a hexadecimal string
 */
BigInt *big_int_create_from_hex(char* s)
{
    BigInt *a;
    size_t s_len;
    int64_t chunk_size, i;
    char buf[BIGINT_CHUNK_HEX_SIZE + 1];
    char *s_end;
    long long parsed_int;

    s_len = strlen(s);
    s_end = s + s_len;
    chunk_size = 1 + s_len / BIGINT_CHUNK_HEX_SIZE;

    // Check if the given string is too large for our BigInts
    if (chunk_size > BIGINT_FIXED_SIZE)
        FATAL("Integer %s is larger than %lu bytes and currently not supported.",
            s, BIGINT_FIXED_SIZE * sizeof(uint64_t));

    // NOTE: Currently, all BigInts have size BIGINT_FIXED_SIZE
    a = big_int_alloc(BIGINT_FIXED_SIZE);

    a->overflow  = 0;
    a->size      = chunk_size;

    // Null terinate buffer to stop strtoll
    buf[BIGINT_CHUNK_HEX_SIZE] = 0;
    for (i = 0; i < chunk_size - 1; ++i) {
        s_end -= BIGINT_CHUNK_HEX_SIZE;
        strncpy(buf, s_end, BIGINT_CHUNK_HEX_SIZE);

        a->chunks[i] = (uint64_t) strtoll(buf, NULL, 16);
    }

    // Parse the last (leftmost) chunk and store the sign bit
    s_len = (size_t) ((uintptr_t) s_end - (uintptr_t) s);
    buf[s_len] = 0;
    strncpy(buf, s, s_len);
    parsed_int = strtoll(buf, NULL, 16);
    a->sign = parsed_int < 0;
    a->chunks[i] = llabs(parsed_int);

    return a;
}


/**
 * \brief Free a BigInt
 */
void big_int_destroy(BigInt *a)
{
    if (a->size > 0)
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

    memcpy((void *) a->chunks, (void *) b->chunks, b->size * sizeof(uint64_t));
}

/**
 * \brief Duplicate a BigInt
 */
BigInt *big_int_duplicate(BigInt *a)
{
    BigInt *b;

    b = big_int_alloc(a->alloc_size);
    big_int_copy(b, a);

    return b;
}


/**
 * \brief Calculate -a
 */
BigInt *big_int_neg(BigInt *a)
{
    BigInt *neg_a;

    neg_a = big_int_duplicate(a);
    neg_a->sign = !a->sign;

    return neg_a;
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
//  * \brief Calculate a * b
//  */
// // TODO: add mul_mod function, might remove this one
// // TODO: change for 256 bits
// BigInt big_int_mul(BigInt a, BigInt b)
// {
//     BigInt r = {a.x * b.x};
//     return r;
// }
//
// /**
//  * \brief Calculate a // b (integer division)
//  */
// // TODO: change for 256 bits
// BigInt big_int_div(BigInt a, BigInt b)
// {
//     BigInt r = {a.x / b.x};
//     return r;
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
//  * \brief Calculate a + b
//  */
// // TODO: add add_mod function, might remove this one
// // BigInt big_int_add_mod(BigInt a, BigInt b, BigInt q)
// // TODO: change for 256 bits
// BigInt big_int_add(BigInt a, BigInt b)
// {
//     BigInt r = {a.x + b.x};
//     return r;
// }
//
//
// /**
//  * \brief Calculate a - b
//  */
// // TODO: add sub_mod function, might remove this one
// // BigInt big_int_sub_mod(BigInt a, BigInt b, BigInt q)
// // TODO: change for 256 bits
// BigInt big_int_sub(BigInt a, BigInt b)
// {
//     BigInt r = {a.x - b.x};
//     return r;
// }
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
//
//
// /**
//  * \brief Calculate a == b.
//  * \returns r, where a==b: r=0, a<b: r<0, a>b: r>0
//  */
// // TODO: change for 256 bits
// uint64_t big_int_compare(BigInt a, BigInt b)
// {
//     if (a.x == b.x)
//         return 0;
//
//     return (a.x < b.x) ? -1 : 1;
// }
//
//
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

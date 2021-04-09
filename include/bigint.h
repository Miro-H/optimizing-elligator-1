#ifndef BIGINT_H_    /* Include guard */
#define BIGINT_H_
#include <stdint.h>

#define BIGINT_FIXED_SIZE 4

/**
 * \brief Internal data structure for BigInts.
 *        Stores integer a_n * (2^64)^n + a_(n-1) * (2^64)^(n-1) + a_0
 *        as chunks (a_n, a_(n-1), ..., a_0), where chunk[0] = a_0.
 */
// NOTE: For now, we assume size <= 4, i.e., only 256 bit integers.
// We do not reallocate, every BigInt has BIGINT_FIXED_SIZE chunks allocated.
// This would need to be changed for arbitrary sized integer operations!
typedef struct BigInts
{
    uint64_t sign : 1;      // O: positive, 1: negative
    uint64_t overflow : 1;  // 1 if operation overflowed
    uint64_t size : 62;     // Number of chunks of the BigInt
    uint64_t *chunks;       // 64-bit chunks in reverse order
} BigInt;

/**
 * Struct for the results of g := gcd(a, b) = xa + yb
 */
typedef struct egcd_results {
    BigInt *g;
    BigInt *y;
    BigInt *x;
} egcd_result;

// Create and destroy
BigInt *big_int_create(int64_t x);
BigInt *big_int_create_from_string(char* s);
void big_int_destroy(BigInt *a);

// Basic arithmetic operations
BigInt *big_int_neg(BigInt *a);
BigInt *big_int_add(BigInt *a, BigInt *b);
BigInt *big_int_sub(BigInt *a, BigInt *b);
BigInt *big_int_mul(BigInt *a, BigInt *b);
BigInt *big_int_div(BigInt *a, BigInt *b);

// Modular arithmetic
BigInt *big_int_mod(BigInt *a, BigInt *q);
// TODO: add: BigInt *big_int_add(BigInt *a, BigInt *b, BigInt *q);
// TODO: add: BigInt *big_int_sub(BigInt *a, BigInt *b, BigInt *q);
// TODO: add: BigInt *big_int_mul_mod(BigInt *a, BigInt *b, BigInt *q);
// TODO: add: BigInt *big_int_div_mod(BigInt *a, BigInt *b, BigInt *q);
BigInt *big_int_inv(BigInt *a, BigInt *q);

// Comparison operations
uint64_t big_int_compare(BigInt *a, BigInt *b); // a==b: 0, a<b: -1, a>b: 1

// Advanced operations
BigInt *big_int_pow(BigInt *base, BigInt *exponent, BigInt *q);
egcd_result egcd(BigInt *a, BigInt *b);
BigInt *chi(BigInt *t, BigInt q);

#endif // BIGINT_H_

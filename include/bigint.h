#ifndef BIGINT_H_    /* Include guard */
#define BIGINT_H_
#include <stdint.h>

/**
 * \brief Internal data structure for BigInts.
 */
// NOTE: For now, we assume size <= 4, i.e., only 256 bit integers.
// We do not reallocate, every BigInt has four chunks allocated.
// This would need to be changed for arbitrary sized integer operations!
typedef struct BigInts
{
    uint64_t sign : 1;      // O: positive, 1: negative
    uint64_t overflow : 1;  // 1 if operation overflowed
    uint64_t size : 62;     // Size of the bigint
    uint64_t *chunks;       // 64-bit chunks
} BigInt;

/**
 * Struct for the results of g := gcd(a, b) = xa + yb
 */
typedef struct egcd_results {
    BigInt g;
    BigInt y;
    BigInt x;
} egcd_result;

// Create and destroy
BigInt create_big_int(uint64_t x);
BigInt create_big_int_string(char* s);
void destroy_big_int(BigInt a);

// Basic arithmetic operations
BigInt big_int_neg(BigInt a);
BigInt big_int_add(BigInt a, BigInt b);
BigInt big_int_sub(BigInt a, BigInt b);
BigInt big_int_mul(BigInt a, BigInt b);
BigInt big_int_div(BigInt a, BigInt b);

// Modular arithmetic
BigInt big_int_mod(BigInt a, BigInt q);
// TODO: add: BigInt big_int_add(BigInt a, BigInt b, BigInt q);
// TODO: add: BigInt big_int_sub(BigInt a, BigInt b, BigInt q);
// TODO: add: BigInt big_int_mul_mod(BigInt a, BigInt b, BigInt q);
// TODO: add: BigInt big_int_div_mod(BigInt a, BigInt b, BigInt q);
BigInt big_int_inv(BigInt a, BigInt q);

// Comparison operations
uint64_t big_int_compare(BigInt a, BigInt b); // a==b: 0, a<b: -1, a>b: 1

// Advanced operations
BigInt big_int_pow(BigInt base, BigInt exponent, BigInt q);
egcd_result egcd(BigInt a, BigInt b);
BigInt chi(BigInt t, BigInt q);

#endif // BIGINT_H_

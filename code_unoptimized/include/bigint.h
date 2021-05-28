#ifndef BIGINT_H_    /* Include guard */
#define BIGINT_H_

#include <inttypes.h>
#include <string.h>
#include "bigint_types.h"

// To work with 4 chunk integers (256 bits) we internally need 9 chunk integers.
// The reason is that intermediate products can be 8 chunks and division performs
// a scaling that may require an additional chunk.
#define BIGINT_FIXED_SIZE ((uint64_t) 17)
#define BIGINT_CHUNK_HEX_SIZE (sizeof(chunk_size_t) * 2)
#define BIGINT_CHUNK_BIT_SIZE (sizeof(chunk_size_t) * 8)

// Change if larger chunks are used
#define STR_TO_CHUNK strtol
#define CHUNK_ABS llabs
#define BIGINT_RADIX (((dbl_chunk_size_t) 1) << (sizeof(chunk_size_t) * 8))
#define BIGINT_RADIX_SIGNED ((int64_t) BIGINT_RADIX)

// Stats collection
#ifdef COLLECT_STATS
#define ADD_STAT_COLLECTION(type) big_int_stats[(type)]++;
#else
#define ADD_STAT_COLLECTION(type)
#endif

typedef uint32_t chunk_size_t;
typedef uint64_t dbl_chunk_size_t;

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
    uint64_t sign : 1;          // O: positive, 1: negative
    uint64_t overflow : 1;      // 1 if operation overflowed (only supported for add/sub)
    uint64_t size : 62;         // Number of chunks used in the BigInt
    uint64_t alloc_size : 62;   // Number of chunks allocated for the BigInt (>= size)
    dbl_chunk_size_t *chunks;   // Chunks of size chunk_size_t in reverse order
} BigInt;

/**
 * Struct for the results of g := gcd(a, b) = xa + yb
 */
typedef struct EgcdResult {
    BigInt *g;
    BigInt *y;
    BigInt *x;
} EgcdResult;

/*
* Struct that tracks usage of BigInt functions
*/
uint64_t big_int_stats[BIGINT_TYPE_LAST];

static dbl_chunk_size_t chunk_zero = 0;
static dbl_chunk_size_t chunk_one = 1;

// Special BigInts, never free those! They cannot be copied.
__attribute__((unused)) static BigInt *big_int_zero = &((BigInt) {
    .sign = 0,
    .overflow = 0,
    .size = 1,
    .alloc_size = BIGINT_FIXED_SIZE,
    .chunks = &chunk_zero,
});
__attribute__((unused)) static BigInt *big_int_one = &((BigInt) {
    .sign = 0,
    .overflow = 0,
    .size = 1,
    .alloc_size = BIGINT_FIXED_SIZE,
    .chunks = &chunk_one,
});
__attribute__((unused)) static BigInt *big_int_min_one = &((BigInt) {
    .sign = 1,
    .overflow = 0,
    .size = 1,
    .alloc_size = BIGINT_FIXED_SIZE,
    .chunks = &chunk_one,
});

//Functions only exposed for Benchmarks
BigInt *big_int_alloc(uint64_t size);
BigInt *big_int_calloc(uint64_t size);
BigInt *big_int_prune_leading_zeros(BigInt *r, BigInt *a);
BigInt *big_int_create_from_dbl_chunk(BigInt *r, dbl_chunk_size_t chunk, uint8_t sign);

// Meta functions
BigInt *big_int_create_from_chunk(BigInt *r, chunk_size_t chunk, uint8_t sign);
BigInt *big_int_create_from_hex(BigInt *r, char* s);
BigInt *big_int_create_random(BigInt *r, int64_t nr_of_chunks);
void big_int_destroy(BigInt *a);
BigInt *big_int_copy(BigInt *a, BigInt *b);
BigInt *big_int_duplicate(BigInt *a);
void big_int_print(BigInt *a);

/**
 * All functions have as first argument the BigInt where they should store
 * the result to. There are the following options:
 *      if r = NULL:  allocate new BigInt, return pointer to new BigInt
 *      otherwise:    use the given BigInt, return pointer to r. Note that if
 *                    r = a, then this is an in-place modification.
 */
// Basic arithmetic operations
BigInt *big_int_neg(BigInt *r, BigInt *a);
BigInt *big_int_abs(BigInt *r, BigInt *a);
BigInt *big_int_add(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_sub(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_mul(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_div(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_div_rem(BigInt *q, BigInt *r, BigInt *a, BigInt *b);

// Shifts
// TODO: consider implementing general shifts
BigInt *big_int_sll_small(BigInt *r, BigInt *a, uint64_t shift);
BigInt *big_int_srl_small(BigInt *r, BigInt *a, uint64_t shift);

// Modular arithmetic
BigInt *big_int_mod(BigInt *r, BigInt *a, BigInt *q);
BigInt *big_int_add_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q);
BigInt *big_int_sub_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q);
BigInt *big_int_mul_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q);
BigInt *big_int_div_mod(BigInt *r, BigInt *a, BigInt *b, BigInt *q);
BigInt *big_int_inv(BigInt *r, BigInt *a, BigInt *q);

// Comparison operations
int8_t big_int_compare(BigInt *a, BigInt *b);
int8_t big_int_is_zero(BigInt *a);
int8_t big_int_is_odd(BigInt *a);

// Advanced operations
BigInt *big_int_pow(BigInt *r, BigInt *b, BigInt *e, BigInt *q);
EgcdResult *big_int_egcd(EgcdResult *r, BigInt *a, BigInt *b);
BigInt *big_int_chi(BigInt *r, BigInt *t, BigInt *q);

void big_int_free_egcd_result(EgcdResult *r);

// Reset stats (use in combination with setting the env variable COLLECT_STATS)
void reset_stats(void);

#endif // BIGINT_H_

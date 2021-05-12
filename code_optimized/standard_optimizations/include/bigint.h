#ifndef BIGINT_H_    /* Include guard */
#define BIGINT_H_

#include <inttypes.h>
#include <string.h>
#include "bigint_types.h"

// To work with 4 chunk integers (256 bits) we internally need 9 chunk integers.
// The reason is that intermediate products can be 8 chunks and division performs
// a scaling that may require an additional chunk.
#define BIGINT_FIXED_SIZE ((uint32_t) 17)
#define BIGINT_METADATA_SIZE ((uint32_t) 2 * sizeof(uint32_t))
#define BIGINT_INTERNAL_CHUNK_BYTE ((uint32_t) sizeof(dbl_chunk_size_t))
#define BIGINT_CHUNK_BYTE_SIZE ((uint32_t) sizeof(chunk_size_t))
#define BIGINT_CHUNK_HEX_SIZE ((uint32_t) sizeof(chunk_size_t) * 2)
#define BIGINT_CHUNK_BIT_SIZE ((uint32_t) sizeof(chunk_size_t) * 8)

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

// NOTE: For efficiency, we assume size <= 4, i.e., only 256 bit integers.
// We do not reallocate, every BigInt has BIGINT_FIXED_SIZE chunks allocated.
// Internally, larger BigInts are possible for intermediate results.

// Attributes:
// - packed: makes sure that the struct has the minimal size (no padding)
// - aligned: makes sure the struct is aligned to 4 bytes
typedef struct __attribute__((packed, aligned(4))) BigInt
{
    uint32_t sign : 1;          // O: positive, 1: negative
    uint32_t overflow : 1;      // 1 if operation overflowed (only supported for add/sub)
    uint32_t size : 30;         // Number of chunks used in the BigInt
    uint32_t pad;               // XXX: remove when switching to chunks of 32 bits
    dbl_chunk_size_t chunks[BIGINT_FIXED_SIZE];// Chunks of size chunk_size_t in reverse order
} BigInt;

/**
 * Struct for the results of g := gcd(a, b) = xa + yb
 */
typedef struct EgcdResult {
    BigInt g;
    BigInt y;
    BigInt x;
} EgcdResult;

/*
* Struct that tracks usage of BigInt functions
*/
uint64_t big_int_stats[BIGINT_TYPE_LAST];

// BIG_INT_DEFINE_STRUCT but allowing you to specify the type (e.g., static or
// normal BigInt)
#define BIG_INT_DEFINE_STRUCT_GENERAL(type, name, sign_, overflow_, size_, _chunk) \
    type name##_bigint = ((BigInt) {                                           \
        .sign = (sign_),                                                       \
        .overflow = (overflow_),                                               \
        .size = (size_),                                                       \
        .chunks = {_chunk},                                                    \
    })

// Define BigInt of fixed size with given parameters and sets the chunks pointer
// to 'chunks'
#define BIG_INT_DEFINE_STRUCT(name, sign, overflow, size, chunks)              \
    BIG_INT_DEFINE_STRUCT_GENERAL(BigInt, name, sign, overflow, size, chunks)

// Define BigInt of fixed size with given parameters and sets the chunks pointer
// to 'chunks'. Moreover, define a pointer for name, pointing to the new BigInt
#define BIG_INT_DEFINE_STRUCT_PTR(name, sign, overflow, size, chunks)          \
    BIG_INT_DEFINE_STRUCT_GENERAL(BigInt, name, sign, overflow, size, chunks); \
    BigInt *name = &name##_bigint

// Define BigInt of fixed size based on given chunk and sign
#define BIG_INT_DEFINE_FROM_CHUNK(name, sign, chunk)                           \
    BIG_INT_DEFINE_STRUCT_PTR(name, sign, 0, 1, chunk);

// Define BigInt and pointer to it without setting any values
#define BIG_INT_DEFINE_PTR(name)                                               \
    BigInt name##_bigint;                                                      \
    BigInt *name = &name##_bigint

// Define BigInt which is zeroed out and pointer to it
#define BIG_INT_DEFINE_PTR_ZEROED(name)                                        \
    BigInt name##_bigint = {0};                                                \
    BigInt *name = &name##_bigint

// Define BigInt of fixed size with given parameters and sets the chunks pointer
// to 'chunks'
#define BIG_INT_DEFINE_STATIC_STRUCT_PTR(name, sign, overflow, size, chunks)   \
    BIG_INT_DEFINE_STRUCT_GENERAL(static BigInt, name, sign, overflow,         \
        size, chunks);                                                         \
    static BigInt *name = &name##_bigint

// Special BigInts, never free those! They cannot be copied.
__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_zero, 0, 0, 1, 0);

__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_one, 0, 0, 1, 1);

__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_min_one, 1, 0, 1, 1);

//Functions only exposed for Benchmarks
BigInt *big_int_prune_leading_zeros(BigInt *r, BigInt *a);

// Meta functions
BigInt *big_int_create_from_dbl_chunk(BigInt *r, dbl_chunk_size_t chunk, uint8_t sign);
BigInt *big_int_create_from_chunk(BigInt *r, chunk_size_t chunk, uint8_t sign);
BigInt *big_int_create_from_hex(BigInt *r, char* s);
BigInt *big_int_create_random(BigInt *r, int64_t nr_of_chunks);
BigInt *big_int_copy(BigInt *a, BigInt *b);
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

// Reset stats (use in combination with setting the env variable COLLECT_STATS)
void reset_stats(void);

#endif // BIGINT_H_

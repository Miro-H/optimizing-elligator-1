#ifndef RUNTIME_BENCHMARK_H_ /* Include guard */
#define RUNTIME_BENCHMARK_H_

#ifndef LOG_PATH
#define LOG_PATH "../logs/runtime"
#endif

#ifndef BIGINTSIZE
// 256 bits
#define BIGINTSIZE 8
#endif

#ifndef SETS
#define SETS 10
#endif

#ifndef REPS
#define REPS 10000
#endif

#include "bigint.h"
#include "elligator.h"

// Create macro to define a BigInt chunk according to the different APIs of
// different versions of our code
#if VERSION == 1

#define RUNTIME_DECLARE_BIGINT_ARRAY(arr_name) BigInt **arr_name

#define RUNTIME_DEREF(b, i) ((b)[(i)])
#define RUNTIME_REF(b) (b)
#define RUNTIME_BIG_INT_DECLARE(b) BigInt *b
#define RUNTIME_BIG_INT_DEFINE(v) BigInt *v = big_int_create_from_chunk(NULL, 0, 0)
#define RUNTIME_BIG_INT_INIT(v) v = big_int_create_from_chunk(NULL, 0, 0)
#define RUNTIME_BIG_INT_ALLOC_ARR(a, arr_size) a = (BigInt **) malloc((arr_size) * sizeof(BigInt *))
#define RUNTIME_BIG_INT_DESTROY(v) big_int_destroy(v)
#define RUNTIME_BIG_INT_CREATE_RANDOM(v, w) v = big_int_create_random(NULL, w)
#define RUNTIME_BIG_INT_FREE_EGCD_RES(r) big_int_free_egcd_result(r)
#define RUNTIME_BIG_INT_CREATE_FROM_HEX(v, w) v = big_int_create_from_hex(NULL, w)

#define RUNTIME_FREE_CURVE_POINT(point) free_curve_point((point))
#define RUNTIME_FREE_CURVE(curve) free_curve((curve));

#else

#define RUNTIME_DECLARE_BIGINT_ARRAY(arr_name) BigInt *arr_name

#define RUNTIME_DEREF(b, i) ((b)+(i))
#define RUNTIME_REF(b) (&b)
#define RUNTIME_BIG_INT_DECLARE(b) BIG_INT_DEFINE_PTR(b)
#define RUNTIME_BIG_INT_DEFINE(v) BIG_INT_DEFINE_FROM_CHUNK(v, 0, 0)
#define RUNTIME_BIG_INT_INIT(v) ((void) 0)
#define RUNTIME_BIG_INT_ALLOC_ARR(a, arr_size) a = (BigInt *) malloc((arr_size) * sizeof(BigInt))
#define RUNTIME_BIG_INT_DESTROY(v) ((void) 0)
#define RUNTIME_BIG_INT_CREATE_RANDOM(v, w) big_int_create_random(v, w)
#define RUNTIME_BIG_INT_FREE_EGCD_RES(r) ((void) 0)
#define RUNTIME_BIG_INT_CREATE_FROM_HEX(v, w) big_int_create_from_hex(v, w)

#define RUNTIME_FREE_CURVE_POINT(point) ((void) 0)
#define RUNTIME_FREE_CURVE(curve) ((void) 0)

#endif


/*
 * Global BigInt variables
 */
RUNTIME_DECLARE_BIGINT_ARRAY(big_int_array);
RUNTIME_DECLARE_BIGINT_ARRAY(big_int_array_1);
RUNTIME_DECLARE_BIGINT_ARRAY(big_int_array_2);
RUNTIME_DECLARE_BIGINT_ARRAY(big_int_array_3);
RUNTIME_DECLARE_BIGINT_ARRAY(big_int_array_4);

RUNTIME_DECLARE_BIGINT_ARRAY(big_int_array_q);

RUNTIME_DECLARE_BIGINT_ARRAY(big_int_64_bit_array);

RUNTIME_BIG_INT_DECLARE(bench_big_int_q_m1_d2); // (q-1)/2
RUNTIME_BIG_INT_DECLARE(bench_big_int_q_p1_d4); // (q+1)/4

EgcdResult *big_int_egcd_array;

Curve bench_curve;
CurvePoint *curve_point_array;

int8_t *int8_t_array_1;
uint64_t *uint64_t_array_1;

int big_int_size_;
int big_int_array_size_;

int bench_big_int_size_256_args[] = {BIGINTSIZE, REPS, 1};
int bench_big_int_size_256_random_mod_args[] = {BIGINTSIZE, REPS, 1};
int bench_big_int_size_256_curve_mod_args[] = {BIGINTSIZE, REPS, 0};
int bench_big_int_size_512_curve_mod_args[] = {2 * BIGINTSIZE, REPS, 0};


#endif // RUNTIME_BENCHMARK_H_

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

#if VERSION == 1
#define RUNTIME_DECLARE_BIGINT_ARRAY(arr_name) BigInt **arr_name
#else
#define RUNTIME_DECLARE_BIGINT_ARRAY(arr_name) BigInt *arr_name
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


#endif // RUNTIME_BENCHMARK_H_

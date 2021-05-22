#ifndef RUNTIME_BENCHMARK_CURVE1174_H_ /* Include guard */
#define RUNTIME_BENCHMARK_CURVE1174_H_

#ifndef LOG_PATH
#define LOG_PATH "../logs/runtime"
#endif

#ifndef SETS
#define SETS 10
#endif

#ifndef REPS
#define REPS 10000
#endif

#include "bigint.h"

/*
 * Global BigInt variables
 */
BigInt *big_int_array_1;
BigInt *big_int_array_2;
BigInt *big_int_array_3;

BigInt *big_int_array_of_ones;

int8_t *int8_t_array;
uint64_t *uint64_t_array;

int big_int_array_size_;

int bench_big_int_curve_1174_args[] = {REPS};


#endif // RUNTIME_BENCHMARK_CURVE1174_H_

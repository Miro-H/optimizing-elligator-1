#ifndef GMP_BENCHMARK_H_ /* Include guard */
#define GMP_BENCHMARK_H_

#ifndef LOG_PATH
#define LOG_PATH "./logs/gmp/runtime"
#endif

#ifndef INTSIZE
// 256 bits
#define INTSIZE 8
#endif

#ifndef SETS
#define SETS 10
#endif

#ifndef REPS
#define REPS 10000
#endif

#include <gmp.h>

/*
 * Global mpz_t variables (GMP int type)
 */
mpz_t *mpz_array;
mpz_t *mpz_array_1;
mpz_t *mpz_array_2;
mpz_t *mpz_array_3;
mpz_t *mpz_array_4;
mpz_t *mpz_array_5;
mpz_t *mpz_array_q;

int8_t *int8_t_array_1;
uint64_t *uint64_t_array_1;

int mpz_size_;

int bench_mpz_size_256_args[] = {INTSIZE, REPS, 1};
int bench_mpz_size_256_random_mod_args[] = {INTSIZE, REPS, 1};
int bench_mpz_size_256_curve_mod_args[] = {INTSIZE, REPS, 0};


#endif //GMP_BENCHMARK_H_

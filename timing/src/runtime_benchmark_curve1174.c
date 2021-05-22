/*
 * Includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h> // for random BigInt

// Include header files
#include "bigint_curve1174.h"
#include "runtime_benchmark_curve1174.h"
#include "debug.h"
#include "benchmark_helpers.h"
#include "benchmark_types.h"

/*
 * Macros
 */
#define RUNTIME_BIG_INT_ALLOC_ARR(a, arr_size) a = (BigInt *) malloc((arr_size) * sizeof(BigInt))

//=== === === === === === === === === === === === === === ===

void bench_big_int_prep(void *argptr)
{
    int64_t array_size = ((int *)argptr)[0];

    RUNTIME_BIG_INT_ALLOC_ARR(big_int_array_1, array_size);
    RUNTIME_BIG_INT_ALLOC_ARR(big_int_array_2, array_size);
    RUNTIME_BIG_INT_ALLOC_ARR(big_int_array_3, array_size);

    RUNTIME_BIG_INT_ALLOC_ARR(big_int_array_of_ones, array_size);

    int8_t_array = (int8_t *) malloc(array_size * sizeof(int8_t));
    uint64_t_array = (uint64_t *) malloc(array_size * sizeof(uint64_t));

    uint64_t offset;
    for (uint64_t i = 0; i < array_size; i++)
    {
        big_int_create_random(big_int_array_1 + i, BIGINT_FIXED_SIZE);
        big_int_create_random(big_int_array_2 + i, BIGINT_FIXED_SIZE);
        big_int_create_random(big_int_array_3 + i, BIGINT_FIXED_SIZE);

        big_int_create_from_chunk(big_int_array_of_ones, 1, 0);

        offset = 1;
        uint64_t_array[i] = 0;
        while (offset < UINT64_MAX / RAND_MAX) {
            uint64_t_array[i] += ((uint64_t) rand()) * offset;
            offset *= RAND_MAX;
        }
    }
}

void bench_big_int_cleanup(void *argptr)
{
    free(big_int_array_1);
    free(big_int_array_2);
    free(big_int_array_3);

    free(int8_t_array);
    free(uint64_t_array);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_add_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_add_mod(big_int_array_1 + i, big_int_array_2 + i,
        big_int_array_3 + i);
}

void bench_big_int_curve1174_add_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_add_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_sub_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_sub_mod(big_int_array_1 + i, big_int_array_2 + i,
        big_int_array_3 + i);
}

void bench_big_int_curve1174_sub_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_sub_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_mul_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_mul_mod(big_int_array_1 + i, big_int_array_2 + i,
        big_int_array_3 + i);
}

void bench_big_int_curve1174_mul_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_mul_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_div_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_div_mod(big_int_array_1 + i, big_int_array_2 + i,
        big_int_array_3 + i);
}

void bench_big_int_curve1174_div_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_div_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_mod(big_int_array_1 + i);
}

void bench_big_int_curve1174_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_inv_fermat_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_inv_fermat(big_int_array_1 + i, big_int_array_2 + i);
}

void bench_big_int_curve1174_inv_fermat(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_inv_fermat_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_pow_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_pow(big_int_array_1 + i, big_int_array_2 + i,
        big_int_array_3 + i);
}

void bench_big_int_curve1174_pow(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_pow_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_pow_small_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_pow_small(big_int_array_1 + i, big_int_array_2 + i,
        uint64_t_array[i]);
}

void bench_big_int_curve1174_pow_small(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_pow_small_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_pow_q_m1_d2_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_pow_q_m1_d2(big_int_array_of_ones + i, big_int_array_2 + i);
}

void bench_big_int_curve1174_pow_q_m1_d2(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_pow_q_m1_d2_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_pow_q_p1_d4_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    big_int_curve1174_pow_q_p1_d4(big_int_array_1 + i, big_int_array_2 + i);
}

void bench_big_int_curve1174_pow_q_p1_d4(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_pow_q_p1_d4_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_compare_to_q_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    int8_t_array[i] = big_int_curve1174_compare_to_q(big_int_array_1 + i);
}

void bench_big_int_curve1174_compare_to_q(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_compare_to_q_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

void bench_big_int_curve1174_chi_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    int8_t_array[i] = big_int_curve1174_chi(big_int_array_1 + i);
}

void bench_big_int_curve1174_chi(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_curve1174_chi_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//=== === === === === === === === === === === === === === ===

int main(int argc, char const *argv[])
{

    int bench_type;
    for (int i = 1; i < argc; ++i) {
        bench_type = atoi(argv[i]);

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_ADD_MOD,
            bench_big_int_curve1174_add_mod(
                (void *) bench_big_int_curve_1174_args,
                "add mod (curve)",
                LOG_PATH "/runtime_big_int_curve_1174_add_mod.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_SUB_MOD,
            bench_big_int_curve1174_sub_mod(
                (void *) bench_big_int_curve_1174_args,
                "sub mod (curve)",
                LOG_PATH "/runtime_big_int_curve_1174_sub_mod.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_MUL_MOD,
            bench_big_int_curve1174_mul_mod(
                (void *) bench_big_int_curve_1174_args,
                "mul mod (curve)",
                LOG_PATH "/runtime_big_int_curve_1174_mul_mod.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_MOD,
            bench_big_int_curve1174_mod(
                (void *) bench_big_int_curve_1174_args,
                "mod (curve)",
                LOG_PATH "/runtime_big_int_curve_1174_mod.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_DIV_MOD,
            bench_big_int_curve1174_div_mod(
                (void *) bench_big_int_curve_1174_args,
                "div mod (curve)",
                LOG_PATH "/runtime_big_int_curve_1174_div_mod.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_INV_FERMAT,
            bench_big_int_curve1174_inv_fermat(
                (void *) bench_big_int_curve_1174_args,
                "inv fermat",
                LOG_PATH "/runtime_big_int_curve_1174_inv_fermat.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_COMPARE,
            bench_big_int_curve1174_compare_to_q(
                (void *) bench_big_int_curve_1174_args,
                "compare to q",
                LOG_PATH "/runtime_big_int_curve1174_compare_to_q.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_POW,
            bench_big_int_curve1174_pow(
                (void *) bench_big_int_curve_1174_args,
                "pow (curve)",
                LOG_PATH "/runtime_big_int_curve1174_pow.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_POW_SMALL,
            bench_big_int_curve1174_pow_small(
                (void *) bench_big_int_curve_1174_args,
                "pow, exp: 64-bit (curve)",
                LOG_PATH "/runtime_big_int_curve1174_pow_small.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_POW_Q_M1_D2,
            bench_big_int_curve1174_pow_q_m1_d2(
                (void *) bench_big_int_curve_1174_args,
                "pow, exp: (q-1)/2 (curve)",
                LOG_PATH "/runtime_big_int_curve1174_pow_q_m1_d2.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_POW_Q_P1_D4,
            bench_big_int_curve1174_pow_q_p1_d4(
                (void *) bench_big_int_curve_1174_args,
                "pow, exp: (q+1)/4 (curve)",
                LOG_PATH "/runtime_big_int_curve1174_pow_q_p1_d4.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CURVE_1174_CHI,
            bench_big_int_curve1174_chi(
                (void *) bench_big_int_curve_1174_args,
                "chi",
                LOG_PATH "/runtime_big_int_curve1174_chi.log"));
    }

    return EXIT_SUCCESS;
}

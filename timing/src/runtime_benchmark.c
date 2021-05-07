/*
 * Includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h> // for random BigInt

// Include header files
#include "runtime_benchmark.h"
#include "bigint.h"
#include "elligator.h"
#include "debug.h"
#include "benchmark_helpers.h"
#include "benchmark_types.h"


//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];

    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_q = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    int8_t_array_1 = (int8_t *)malloc(big_int_array_size_ * sizeof(int8_t));
    uint64_t_array_1 = (uint64_t *)malloc(big_int_array_size_ * sizeof(uint64_t));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {

        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);

        if (random_q)
        {
            big_int_array_q[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        }
        else
        {
            big_int_array_q[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }

        uint64_t_array_1[i] = rand() % 256;
    }
}

// Run after benchmark
void bench_big_int_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
        big_int_destroy(big_int_array_3[i]);
        big_int_destroy(big_int_array_4[i]);
        big_int_destroy(big_int_array_q[i]);
    }
    free(big_int_array);
    free(big_int_array_1);
    free(big_int_array_2);
    free(big_int_array_3);
    free(big_int_array_4);
    free(big_int_array_q);

    free(int8_t_array_1);
    free(uint64_t_array_1);
}


void bench_big_int_small_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
    }
}

// Run after benchmark
void bench_big_int_small_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
        big_int_destroy(big_int_array_1[i]);
    }
    free(big_int_array);
    free(big_int_array_1);
}


void bench_big_int_destroy_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
    }
}

void bench_big_int_destroy_cleanup(void)
{
    free(big_int_array);
}


void bench_elligator_1_string_to_point_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    init_curve1174(&bench_curve);
    curve_point_array = (CurvePoint *)malloc(big_int_array_size_ * sizeof(CurvePoint));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
    }
}

// Run after benchmark
void bench_elligator_1_string_to_point_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
    }
    free(curve_point_array);
    free(big_int_array_1);
    free_curve(&bench_curve);
}


void bench_elligator_1_point_to_string_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    init_curve1174(&bench_curve);
    curve_point_array = (CurvePoint *)malloc(big_int_array_size_ * sizeof(CurvePoint));


    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        curve_point_array[i] = elligator_1_string_to_point(big_int_array_1[i], bench_curve);;
    }
}

// Run after benchmark
void bench_elligator_1_point_to_string_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
    }
    free(curve_point_array);
    free(big_int_array_1);
    free_curve(&bench_curve);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_alloc_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
    }
}

void bench_big_int_alloc(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_small_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_calloc_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_calloc(big_int_size_);
    }
}

void bench_big_int_calloc(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_small_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_destroy_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
}



void bench_big_int_destroy(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_copy_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_copy(big_int_array_1[i], big_int_array_2[i]);
    }
}

void bench_big_int_copy(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_prune_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_prune_leading_zeros(big_int_array_1[i], big_int_array_1[i]);
    }
}

void bench_big_int_prune(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_prune_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create(NULL, 1);
    }
}

void bench_big_int_create(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_small_prep,
        .bench_fn = bench_big_int_create_fn,
        .bench_cleanup_fn = bench_big_int_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_from_dbl_chunk_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_from_dbl_chunk(NULL, 1, 1);
    }
}

void bench_big_int_create_from_dbl_chunk(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_small_prep,
        .bench_fn = bench_big_int_create_from_dbl_chunk_fn,
        .bench_cleanup_fn = bench_big_int_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_from_hex_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_from_hex(NULL, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    }
}

void bench_big_int_create_from_hex(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_create_from_hex_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_random_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

void bench_big_int_create_random(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_small_prep,
        .bench_fn = bench_big_int_create_random_fn,
        .bench_cleanup_fn = bench_big_int_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_duplicate_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_duplicate(big_int_array_1[i]);
    }
}

void bench_big_int_duplicate(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_small_prep,
        .bench_fn = bench_big_int_duplicate_fn,
        .bench_cleanup_fn = bench_big_int_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_neg_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_neg(big_int_array_1[i], big_int_array_1[i]);
    }
}

void bench_big_int_neg(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_neg_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_abs_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_abs(big_int_array_1[i], big_int_array_1[i]);
    }
}

void bench_big_int_abs(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_abs_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_add_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_add(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

void bench_big_int_add(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_add_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_add_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_add_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_q[i]);
    }
}

void bench_big_int_add_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_add_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_sub_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_sub(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

void bench_big_int_sub(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_sub_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_sub_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_sub_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_q[i]);
    }
}

void bench_big_int_sub_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_sub_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_mul_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_mul(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

void bench_big_int_mul(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_mul_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_mul_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_mul_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_q[i]);
    }
}

void bench_big_int_mul_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_mul_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_div_rem_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_div_rem(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_q[i]);
    }
}

void bench_big_int_div_rem(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_div_rem_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_div_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_div(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

void bench_big_int_div(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_div_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_div_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_div_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_q[i]);
    }
}

void bench_big_int_div_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_div_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_sll_small_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_sll_small(big_int_array_1[i], big_int_array_2[i], uint64_t_array_1[i]);
    }
}

void bench_big_int_sll_small(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_sll_small_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

// Benchmark itself
void bench_big_int_srl_small_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_srl_small(big_int_array_1[i], big_int_array_2[i], uint64_t_array_1[i]);
    }
}

void bench_big_int_srl_small(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_srl_small_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_q[i]);
    }
}

void bench_big_int_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_mod_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_inv_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_inv(big_int_array_1[i], big_int_array_2[i], big_int_array_q[i]);
    }
}

void bench_big_int_inv(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_inv_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_pow_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_pow(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_q[i]);
    }
}

void bench_big_int_pow(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_pow_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_is_zero_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        int8_t_array_1[i] = big_int_is_zero(big_int_array_1[i]);
    }
}

void bench_big_int_is_zero(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_is_zero_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_is_odd_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        int8_t_array_1[i] = big_int_is_odd(big_int_array_1[i]);
    }
}

void bench_big_int_is_odd(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_is_odd_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_compare_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        int8_t_array_1[i] = big_int_compare(big_int_array_1[i], big_int_array_2[i]);
    }
}

void bench_big_int_compare(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_compare_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_egcd_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_egcd(big_int_array_1[i], big_int_array_2[i]);
    }
}

void bench_big_int_egcd(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_egcd_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_chi_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_chi(big_int_array_1[i], big_int_array_2[i], big_int_array_q[i]);
    }
}

void bench_big_int_chi(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_big_int_chi_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_elligator_1_string_to_point_fn(void)
{

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        curve_point_array[i] = elligator_1_string_to_point(big_int_array_1[i], bench_curve);
    }
}

void bench_elligator_1_string_to_point(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_elligator_1_string_to_point_prep,
        .bench_fn = bench_elligator_1_string_to_point_fn,
        .bench_cleanup_fn = bench_elligator_1_string_to_point_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_elligator_1_point_to_string_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = elligator_1_point_to_string(curve_point_array[i], bench_curve);
    }
}

void bench_elligator_1_point_to_string(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_elligator_1_point_to_string_prep,
        .bench_fn = bench_elligator_1_point_to_string_fn,
        .bench_cleanup_fn = bench_elligator_1_point_to_string_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

int main(int argc, char const *argv[])
{

    int bench_type;
    for (int i = 1; i < argc; ++i) {
        bench_type = atoi(argv[i]);

        BENCHMARK(bench_type, BENCH_TYPE_ALLOC,
            bench_big_int_alloc((void *)bench_big_int_size_256_args, "alloc",
                LOG_PATH "/runtime_big_int_alloc_prep.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CALLOC,
            bench_big_int_calloc((void *)bench_big_int_size_256_args, "calloc",
                LOG_PATH "/runtime_big_int_calloc_prep.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DESTROY,
            bench_big_int_destroy((void *)bench_big_int_size_256_args, "destroy",
                LOG_PATH "/runtime_big_int_destroy_prep.log"));

        BENCHMARK(bench_type, BENCH_TYPE_COPY,
            bench_big_int_copy((void *)bench_big_int_size_256_args, "copy",
                LOG_PATH "/runtime_big_int_copy_prep.log"));

        BENCHMARK(bench_type, BENCH_TYPE_PRUNE,
            bench_big_int_prune((void *)bench_big_int_size_256_args, "prune",
                LOG_PATH "/runtime_big_int_prune_prep.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CREATE,
            bench_big_int_create((void *)bench_big_int_size_256_args, "create",
                LOG_PATH "/runtime_big_int_create.log"););

        BENCHMARK(bench_type, BENCH_TYPE_CREATE_DBL_CHUNK,
            bench_big_int_create_from_dbl_chunk((void *)bench_big_int_size_256_args,
                "create from double chunk",
                LOG_PATH "/runtime_big_int_create_from_dbl_chunk.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CREATE_HEX,
            bench_big_int_create_from_hex((void *)bench_big_int_size_256_args,
                "create from hex", LOG_PATH "/runtime_big_int_create_from_hex.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CREATE_RANDOM,
            bench_big_int_create_random((void *)bench_big_int_size_256_args,
                "create random", LOG_PATH "/runtime_big_int_create_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DUPLICATE,
            bench_big_int_duplicate((void *)bench_big_int_size_256_args, "duplicate",
                LOG_PATH "/runtime_big_int_duplicate.log"));

        BENCHMARK(bench_type, BENCH_TYPE_NEG,
            bench_big_int_neg((void *)bench_big_int_size_256_args, "negate",
                LOG_PATH "/runtime_big_int_negate.log"));

        BENCHMARK(bench_type, BENCH_TYPE_ABS,
            bench_big_int_abs((void *)bench_big_int_size_256_args, "abs",
                LOG_PATH "/runtime_big_int_abs.log"));

        BENCHMARK(bench_type, BENCH_TYPE_ADD,
            bench_big_int_add((void *)bench_big_int_size_256_args,
                "add", LOG_PATH "/runtime_big_int_add.log"));

        BENCHMARK(bench_type, BENCH_TYPE_ADD_CURVE,
            bench_big_int_add_mod((void *)bench_big_int_size_256_curve_mod_args,
                "add mod (curve)", LOG_PATH "/runtime_big_int_add_mod_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_ADD_RANDOM,
            bench_big_int_add_mod((void *)bench_big_int_size_256_random_mod_args,
                "add mod (random)", LOG_PATH "/runtime_big_int_add_mod_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_SUB,
            bench_big_int_sub((void *)bench_big_int_size_256_args,
                "sub", LOG_PATH "/runtime_big_int_sub.log"));

        BENCHMARK(bench_type, BENCH_TYPE_SUB_CURVE,
            bench_big_int_sub_mod((void *)bench_big_int_size_256_curve_mod_args,
                "sub mod (curve)", LOG_PATH "/runtime_big_int_sub_mod_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_SUB_RANDOM,
            bench_big_int_sub_mod((void *)bench_big_int_size_256_random_mod_args,
                "sub mod (random)", LOG_PATH "/runtime_big_int_sub_mod_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_MUL,
            bench_big_int_mul((void *)bench_big_int_size_256_args, "mul",
                LOG_PATH "/runtime_big_int_mul.log"));

        BENCHMARK(bench_type, BENCH_TYPE_MUL_CURVE,
            bench_big_int_mul_mod((void *)bench_big_int_size_256_curve_mod_args,
                "mul mod (curve)", LOG_PATH "/runtime_big_int_mul_mod_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_MUL_RANDOM,
            bench_big_int_mul_mod((void *)bench_big_int_size_256_random_mod_args,
                "mul mod (random)", LOG_PATH "/runtime_big_int_mul_mod_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DIVREM,
            bench_big_int_div_rem((void *)bench_big_int_size_256_random_mod_args,
                "divrem", LOG_PATH "/runtime_big_int_div_rem_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DIVREM_CURVE,
            bench_big_int_div_rem((void *)bench_big_int_size_256_curve_mod_args,
                "divrem (curve)", LOG_PATH "/runtime_big_int_div_rem_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DIVREM_RANDOM,
            bench_big_int_div_rem((void *)bench_big_int_size_256_random_mod_args,
                "divrem (random)", LOG_PATH "/runtime_big_int_div_rem_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DIV,
            bench_big_int_div((void *)bench_big_int_size_256_args, "div",
                LOG_PATH "/runtime_big_int_div.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DIV_CURVE,
            bench_big_int_div((void *)bench_big_int_size_256_curve_mod_args,
                "div (curve)", LOG_PATH "/runtime_big_int_div_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_DIV_RANDOM,
            bench_big_int_div((void *)bench_big_int_size_256_random_mod_args,
                "div (random)", LOG_PATH "/runtime_big_int_div_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_SLL,
            bench_big_int_sll_small((void *)bench_big_int_size_256_args, "sll",
                LOG_PATH "/runtime_big_int_sll_small.log"));

        BENCHMARK(bench_type, BENCH_TYPE_SRL,
            bench_big_int_srl_small((void *)bench_big_int_size_256_args, "srl",
                LOG_PATH "/runtime_big_int_srl_small.log"));

        BENCHMARK(bench_type, BENCH_TYPE_MOD_CURVE,
            bench_big_int_mod((void *)bench_big_int_size_256_curve_mod_args,
            "mod (curve)", LOG_PATH "/runtime_big_int_mod_curve.log"););

        BENCHMARK(bench_type, BENCH_TYPE_MOD_RANDOM,
            bench_big_int_mod((void *)bench_big_int_size_256_random_mod_args,
            "mod (random)", LOG_PATH "/runtime_big_int_mod_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_INV,
            bench_big_int_inv((void *)bench_big_int_size_256_curve_mod_args, "inv",
                LOG_PATH "/runtime_big_int_inv.log"));

        BENCHMARK(bench_type, BENCH_TYPE_POW_CURVE,
            bench_big_int_pow((void *)bench_big_int_size_256_curve_mod_args,
                "pow (curve)", LOG_PATH "/runtime_big_int_pow_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_POW_RANDOM,
            bench_big_int_pow((void *)bench_big_int_size_256_random_mod_args,
                "pow (random)", LOG_PATH "/runtime_big_int_pow_random.log"));

        BENCHMARK(bench_type, BENCH_TYPE_IS_ZERO,
            bench_big_int_is_zero((void *)bench_big_int_size_256_args,
            "check if zero", LOG_PATH "/runtime_big_int_is_zero.log"));

        BENCHMARK(bench_type, BENCH_TYPE_IS_ODD,
            bench_big_int_is_odd((void *)bench_big_int_size_256_args,
            "check if odd", LOG_PATH "/runtime_big_int_is_odd.log"));

        BENCHMARK(bench_type, BENCH_TYPE_COMPARE,
            bench_big_int_compare((void *)bench_big_int_size_256_args, "compare",
                LOG_PATH "/runtime_big_int_compare.log"));

        BENCHMARK(bench_type, BENCH_TYPE_EGCD,
            bench_big_int_egcd((void *)bench_big_int_size_256_args, "eGCD",
                LOG_PATH "/runtime_big_int_egcd.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CHI,
            bench_big_int_chi((void *)bench_big_int_size_256_curve_mod_args, "chi",
                LOG_PATH "/runtime_big_int_chi_curve.log"));

        BENCHMARK(bench_type, BENCH_TYPE_ELLIGATOR1_STR2PNT,
            bench_elligator_1_string_to_point((void *)bench_big_int_size_256_args,
                "Elligator str2pnt",
                LOG_PATH "/runtime_elligator_1_string_to_point.log"));

        BENCHMARK(bench_type, BENCH_TYPE_ELLIGATOR1_PNT2STR,
            bench_elligator_1_point_to_string((void *)bench_big_int_size_256_args,
                "Elligator pnt2str",
                LOG_PATH "/runtime_elligator_1_point_to_string.log"));
    }

    return EXIT_SUCCESS;
}

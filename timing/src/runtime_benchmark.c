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
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_elligator_1_string_to_point_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
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
        .bench_prep_fn = bench_big_int_prep,
        .bench_fn = bench_elligator_1_point_to_string_fn,
        .bench_cleanup_fn = bench_big_int_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, bench_name,
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

int main(void)
{
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_alloc((void *)bench_big_int_size_256_args, "alloc",
        LOG_PATH "/bench_big_int_alloc_prep.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_calloc((void *)bench_big_int_size_256_args, "calloc",
        LOG_PATH "/bench_big_int_calloc_prep.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_destroy((void *)bench_big_int_size_256_args, "destroy",
        LOG_PATH "/bench_big_int_destroy_prep.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_copy((void *)bench_big_int_size_256_args, "copy",
        LOG_PATH "/bench_big_int_copy_prep.log");

    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_prune((void *)bench_big_int_size_256_args, "prune",
        LOG_PATH "/bench_big_int_prune_prep.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create((void *)bench_big_int_size_256_args, "create",
        LOG_PATH "/bench_big_int_create.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_from_dbl_chunk((void *)bench_big_int_size_256_args,
        "create from double chunk", LOG_PATH "/bench_big_int_create_from_dbl_chunk.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_from_hex((void *)bench_big_int_size_256_args,
        "create from hex", LOG_PATH "/bench_big_int_create_from_hex.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_random((void *)bench_big_int_size_256_args,
        "create random", LOG_PATH "/bench_big_int_create_random.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_duplicate((void *)bench_big_int_size_256_args, "duplicate",
        LOG_PATH "/bench_big_int_duplicate.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_neg((void *)bench_big_int_size_256_args, "negate",
        LOG_PATH "/bench_big_int_negate.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_abs((void *)bench_big_int_size_256_args, "abs",
        LOG_PATH "/bench_big_int_abs.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_add((void *)bench_big_int_size_256_args,
        "add", LOG_PATH "/bench_big_int_add.log");
    bench_big_int_add_mod((void *)bench_big_int_size_256_random_mod_args,
        "add mod (random)", LOG_PATH "/bench_big_int_add_mod_random.log");
    bench_big_int_add_mod((void *)bench_big_int_size_256_curve_mod_args,
        "add mod (curve)", LOG_PATH "/bench_big_int_add_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_sub((void *)bench_big_int_size_256_args,
        "sub", LOG_PATH "/bench_big_int_sub.log");
    bench_big_int_sub_mod((void *)bench_big_int_size_256_random_mod_args,
        "sub mod (random)", LOG_PATH "/bench_big_int_sub_mod_random.log");
    bench_big_int_sub_mod((void *)bench_big_int_size_256_curve_mod_args,
        "sub mod (curve)", LOG_PATH "/bench_big_int_sub_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_mul((void *)bench_big_int_size_256_args, "mul",
        LOG_PATH "/bench_big_int_mul.log");
    bench_big_int_mul_mod((void *)bench_big_int_size_256_random_mod_args,
        "mul mod (random)", LOG_PATH "/bench_big_int_mul_mod_random.log");
    bench_big_int_mul_mod((void *)bench_big_int_size_256_curve_mod_args,
        "mul mod (curve)", LOG_PATH "/bench_big_int_mul_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_div_rem((void *)bench_big_int_size_256_random_mod_args,
        "divrem", LOG_PATH "/bench_big_int_div_rem_random.log");
    bench_big_int_div_rem((void *)bench_big_int_size_256_random_mod_args,
        "divrem (random)", LOG_PATH "/bench_big_int_div_rem_random.log");
    bench_big_int_div_rem((void *)bench_big_int_size_256_curve_mod_args,
        "divrem (curve)", LOG_PATH "/bench_big_int_div_rem_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_div((void *)bench_big_int_size_256_args, "div",
        LOG_PATH "/bench_big_int_div.log");
    bench_big_int_div((void *)bench_big_int_size_256_random_mod_args,
        "div (random)", LOG_PATH "/bench_big_int_div_random.log");
    bench_big_int_div((void *)bench_big_int_size_256_curve_mod_args,
        "div (curve)", LOG_PATH "/bench_big_int_div_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_sll_small((void *)bench_big_int_size_256_args, "sll",
        LOG_PATH "/bench_big_int_sll_small.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_srl_small((void *)bench_big_int_size_256_args, "srl",
        LOG_PATH "/bench_big_int_srl_small.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_mod((void *)bench_big_int_size_256_random_mod_args, "mod (random)",
        LOG_PATH "/bench_big_int_mod_random.log");
    bench_big_int_mod((void *)bench_big_int_size_256_curve_mod_args, "mod (curve)",
        LOG_PATH "/bench_big_int_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_inv((void *)bench_big_int_size_256_curve_mod_args, "inv",
        LOG_PATH "/bench_big_int_inv.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_pow((void *)bench_big_int_size_256_random_mod_args,
        "pow (random)", LOG_PATH "/bench_big_int_pow_random.log");
    bench_big_int_pow((void *)bench_big_int_size_256_curve_mod_args,
        "pow (curve)", LOG_PATH "/bench_big_int_pow_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_is_zero((void *)bench_big_int_size_256_args, "check if zero",
        LOG_PATH "/bench_big_int_is_zero.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_is_odd((void *)bench_big_int_size_256_args, "check if odd",
        LOG_PATH "/bench_big_int_is_odd.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_compare((void *)bench_big_int_size_256_args, "compare",
        LOG_PATH "/bench_big_int_compare.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_egcd((void *)bench_big_int_size_256_args, "eGCD",
        LOG_PATH "/bench_big_int_egcd.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_chi((void *)bench_big_int_size_256_curve_mod_args, "chi",
        LOG_PATH "/bench_big_int_chi_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    // TODO: Outsource elligator benchmarks to their own file.
    bench_elligator_1_string_to_point((void *)bench_big_int_size_256_args,
        "Elligator str2pnt", LOG_PATH "/bench_elligator_1_string_to_point.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_elligator_1_point_to_string((void *)bench_big_int_size_256_args,
        "Elligator pnt2str", LOG_PATH "/bench_elligator_1_point_to_string.log");

    // BigInt *test = big_int_create_random(NULL, 8, 31);
    // big_int_print(test);
    // printf("\n");
    // test = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    // big_int_print(test);
    // printf("\n");

    return EXIT_SUCCESS;
}

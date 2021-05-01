/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements benchmarks of our implementation.
 */

/*
 * Includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// Include header files
#include "bigint.h"
#include "debug.h"
#include "benchmark_helpers.h"

#ifndef LOG_PATH
#define LOG_PATH "../logs"
#endif

/*
 * Global BigInt variables
 */
BigInt *i1, *i2, *i3;
BigInt **big_int_array;
BigInt **big_int_array_1;
BigInt **big_int_array_2;
BigInt **big_int_array_3;
BigInt **big_int_array_4;

int big_int_size_;
int big_int_array_size_;

/**
 * \brief Benchmark the chi function
 * TODO: find appropriate input sizes
*/
// Run before benchmark
void bench_chi_prep(void *argptr)
{
    char **bigint_strings;

    bigint_strings = (char **)argptr;
    i1 = big_int_create_from_hex(NULL, bigint_strings[0]);
    i2 = big_int_create_from_hex(NULL, bigint_strings[1]);
    i3 = big_int_create_from_hex(NULL, bigint_strings[2]);
}

// Benchmark itself
void bench_chi_fn(void)
{
    // TODO: is big_int_chi so slow or is there a bug?
    // big_int_chi(i1, i2, i3);
}

// Run after benchmark
void bench_chi_cleanup(void)
{
    big_int_destroy(i1);
    big_int_destroy(i2);
    big_int_destroy(i3);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_alloc_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
}

// Benchmark itself
void bench_big_int_alloc_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
    }
}

// Run after benchmark
void bench_big_int_alloc_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_alloc(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_alloc_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_alloc_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_calloc_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
}

// Benchmark itself
void bench_big_int_calloc_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_calloc(big_int_size_);
    }
}

// Run after benchmark
void bench_big_int_calloc_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_calloc(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_calloc_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_calloc_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

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

// Benchmark itself
void bench_big_int_destroy_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
}

// Run after benchmark
void bench_big_int_destroy_cleanup(void)
{
    free(big_int_array);
}

void bench_big_int_destroy(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_copy_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_copy_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_copy(big_int_array_1[i], big_int_array_2[i]); //will throw error right now since we only support fixed size int
    }
}

// Run after benchmark
void bench_big_int_copy_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
}

void bench_big_int_copy(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_copy_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_copy_fn,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_prune_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, 1, 1);
    }
}

// Benchmark itself
void bench_big_int_prune_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_prune_leading_zeros(big_int_array[i], big_int_array[i]); 
    }
}

// Run after benchmark
void bench_big_int_prune_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_prune(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_prune_prep,
        .bench_fn = bench_big_int_prune_fn,
        .bench_cleanup_fn = bench_big_int_prune_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_prep(void *argptr)
{
    big_int_size_ = 4;
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
}

// Benchmark itself
void bench_big_int_create_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create(NULL, 1);
    }
}

// Run after benchmark
void bench_big_int_create_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_create(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_create_prep,
        .bench_fn = bench_big_int_create_fn,
        .bench_cleanup_fn = bench_big_int_create_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_from_dbl_chunk_prep(void *argptr)
{
    big_int_size_ = 4;
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
}

// Benchmark itself
void bench_big_int_create_from_dbl_chunk_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_from_dbl_chunk(NULL, 1, 1);
    }
}

// Run after benchmark
void bench_big_int_create_from_dbl_chunk_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_create_from_dbl_chunk(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_create_from_dbl_chunk_prep,
        .bench_fn = bench_big_int_create_from_dbl_chunk_fn,
        .bench_cleanup_fn = bench_big_int_create_from_dbl_chunk_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_from_hex_prep(void *argptr)
{
    big_int_size_ = 4;
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
}

// Benchmark itself
void bench_big_int_create_from_hex_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_from_hex(NULL, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    }
}

// Run after benchmark
void bench_big_int_create_from_hex_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_create_from_hex(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_create_from_hex_prep,
        .bench_fn = bench_big_int_create_from_hex_fn,
        .bench_cleanup_fn = bench_big_int_create_from_hex_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_random_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
}

// Benchmark itself
void bench_big_int_create_random_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
    }
}

// Run after benchmark
void bench_big_int_create_random_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_create_random(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_create_random_prep,
        .bench_fn = bench_big_int_create_random_fn,
        .bench_cleanup_fn = bench_big_int_create_random_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_duplicate_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_duplicate_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_2[i] = big_int_duplicate(big_int_array_1[i]);
    }
}

// Run after benchmark
void bench_big_int_duplicate_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
}

void bench_big_int_duplicate(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_duplicate_prep,
        .bench_fn = bench_big_int_duplicate_fn,
        .bench_cleanup_fn = bench_big_int_duplicate_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_neg_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_neg_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_neg(big_int_array[i], big_int_array[i]);
    }
}

// Run after benchmark
void bench_big_int_neg_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_neg(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_neg_prep,
        .bench_fn = bench_big_int_neg_fn,
        .bench_cleanup_fn = bench_big_int_neg_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_abs_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_abs_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_abs(big_int_array[i], big_int_array[i]);
    }
}

// Run after benchmark
void bench_big_int_abs_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

void bench_big_int_abs(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_abs_prep,
        .bench_fn = bench_big_int_abs_fn,
        .bench_cleanup_fn = bench_big_int_abs_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_add_prep(void *argptr)
{
    big_int_size_ = 4;//((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 31);
    }
}

// Benchmark itself
void bench_big_int_add_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_add(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_add_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
        big_int_destroy(big_int_array_3[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
    free(big_int_array_3);
}

void bench_big_int_add(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_add_prep,
        .bench_fn = bench_big_int_add_fn,
        .bench_cleanup_fn = bench_big_int_add_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_add_mod_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    BigInt *q = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 31);
        if(random_q)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = q;
        }
    }
}

// Benchmark itself
void bench_big_int_add_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_add_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
    }
}

// Run after benchmark
void bench_big_int_add_mod_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
        big_int_destroy(big_int_array_3[i]);
        big_int_destroy(big_int_array_4[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
    free(big_int_array_3);
    free(big_int_array_4);
}

void bench_big_int_add_mod(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_add_mod_prep,
        .bench_fn = bench_big_int_add_mod_fn,
        .bench_cleanup_fn = bench_big_int_add_mod_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "unopt.",
                                             path, 50, 1000);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


int main(void)
{
    /*
     * BigInt Chi Benchmark
     */

    // a is no square mod q, i.e., big_int_chi(r, a, q) = -1 mod q
    char *bench_chi_prep_args[] = {
        "0",                        // r
        "A994F8591AF2F272B64058F",  // a
        "68EFDACEEE191D78017E4503", // q
    };

    BenchmarkClosure bench_chi_closure = {
        .bench_prep_args = (void *)bench_chi_prep_args,
        .bench_prep_fn = bench_chi_prep,
        .bench_fn = bench_chi_fn,
        .bench_cleanup_fn = bench_chi_cleanup,
    };

    benchmark_runner(bench_chi_closure, "unopt.",
                     LOG_PATH "/bench_chi.log", 50, 1000);

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
      
    int bench_big_int_size_4_args[] = {8, 64};
    int bench_big_int_size_4_random_mod_args[] = {8, 64, 1};
    int bench_big_int_size_4_curve_mod_args[] = {8, 64, 0};
    /*
    int bench_big_int_size_16_args[] = {16, 64};
    int bench_big_int_size_64_args[] = {64, 64};
    int bench_big_int_size_256_args[] = {256, 64};
    int bench_big_int_size_1024_args[] = {1024, 64};
    
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_alloc((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_alloc_prep_4.log");
    bench_big_int_alloc((void *)bench_big_int_size_16_args, LOG_PATH "/bench_big_int_alloc_prep_16.log");
    bench_big_int_alloc((void *)bench_big_int_size_64_args, LOG_PATH "/bench_big_int_alloc_prep_64.log");
    bench_big_int_alloc((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_alloc_prep_256.log");
    bench_big_int_alloc((void *)bench_big_int_size_1024_args, LOG_PATH "/bench_big_int_alloc_prep_1024.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_calloc((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_calloc_prep_4.log");
    bench_big_int_calloc((void *)bench_big_int_size_16_args, LOG_PATH "/bench_big_int_calloc_prep_16.log");
    bench_big_int_calloc((void *)bench_big_int_size_64_args, LOG_PATH "/bench_big_int_calloc_prep_64.log");
    bench_big_int_calloc((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_calloc_prep_256.log");
    bench_big_int_calloc((void *)bench_big_int_size_1024_args, LOG_PATH "/bench_big_intc_calloc_prep_1024.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_destroy((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_destroy_prep_4.log");
    bench_big_int_destroy((void *)bench_big_int_size_16_args, LOG_PATH "/bench_big_int_destroy_prep_16.log");
    bench_big_int_destroy((void *)bench_big_int_size_64_args, LOG_PATH "/bench_big_int_destroy_prep_64.log");
    bench_big_int_destroy((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_destroy_prep_256.log");
    bench_big_int_destroy((void *)bench_big_int_size_1024_args, LOG_PATH "/bench_big_intc_destroy_prep_1024.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_copy((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_copy_prep_4.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_prune((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_prune_prep_4.log");
    bench_big_int_prune((void *)bench_big_int_size_16_args, LOG_PATH "/bench_big_int_prune_prep_16.log");
    bench_big_int_prune((void *)bench_big_int_size_64_args, LOG_PATH "/bench_big_int_prune_prep_64.log");
    bench_big_int_prune((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_prune_prep_256.log");
    bench_big_int_prune((void *)bench_big_int_size_1024_args, LOG_PATH "/bench_big_int_prune_prep_1024.log");
    
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_create.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_from_dbl_chunk((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_create_from_dbl_chunk.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_from_hex((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_create_from_hex.log");
    
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_random((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_create_random_4.log");
    //bench_big_int_create_random((void *)bench_big_int_size_16_args, LOG_PATH "/bench_big_int_create_random_16.log");
    //bench_big_int_create_random((void *)bench_big_int_size_64_args, LOG_PATH "/bench_big_int_create_random_64.log");
    //bench_big_int_create_random((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_create_random_256.log");
    //bench_big_int_create_random((void *)bench_big_int_size_1024_args, LOG_PATH "/bench_big_int_create_random_1024.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_duplicate((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_duplicate_4.log");
    //bench_big_int_duplicate((void *)bench_big_int_size_16_args, LOG_PATH "/bench_big_int_duplicate_16.log");
    //bench_big_int_duplicate((void *)bench_big_int_size_64_args, LOG_PATH "/bench_big_int_duplicate_64.log");
    //bench_big_int_duplicate((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_duplicate_256.log");
    //bench_big_int_duplicate((void *)bench_big_int_size_1024_args, LOG_PATH "/bench_big_int_duplicate_1024.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_neg((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_negate.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_abs((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_abs.log");
    
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
*/
    bench_big_int_add((void *)bench_big_int_size_4_args, LOG_PATH "/bench_big_int_add.log");
    bench_big_int_add_mod((void *)bench_big_int_size_4_random_mod_args, LOG_PATH "/bench_big_int_add_mod_random.log");
    bench_big_int_add_mod((void *)bench_big_int_size_4_curve_mod_args, LOG_PATH "/bench_big_int_add_mod_curve.log");
    

    BigInt *test = big_int_create_random(NULL, 8,4,31);
    big_int_print(test);
    printf("\n");
    test = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    big_int_print(test);
    printf("\n");

    return EXIT_SUCCESS;
}

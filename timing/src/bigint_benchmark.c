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
#include <time.h> // for random BigInt

// Include header files
#include "bigint.h"
#include "elligator.h"
#include "debug.h"
#include "benchmark_helpers.h"

#ifndef LOG_PATH
#define LOG_PATH "../logs"
#endif

#ifndef SETS
#define SETS 10
#endif

#ifndef REPS
#define REPS 1000
#endif

/*
 * Global BigInt variables
 */
BigInt **big_int_array;
BigInt **big_int_array_1;
BigInt **big_int_array_2;
BigInt **big_int_array_3;
BigInt **big_int_array_4;
Curve bench_curve;
CurvePoint *curve_point_array;

int8_t *int8_t_array_1;
uint64_t *uint64_t_array_1;

int big_int_size_;
int big_int_array_size_;

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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_alloc",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_calloc",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_destroy",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
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
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_copy",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_prune_prep(void *argptr)
{
    int64_t leading_zero_chunks, i, j;

    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    srand(time(NULL));

    for (i = 0; i < big_int_array_size_; i++)
    {
        leading_zero_chunks = rand() % big_int_size_;
        // Force a random number of leading zero bits in the MSB chunk
        big_int_array[i] = big_int_create_random(NULL,
                            big_int_size_ - leading_zero_chunks,
                            rand() % BIGINT_CHUNK_BIT_SIZE);

        assert(big_int_array[i]->alloc_size >= big_int_size_);

        // Create a random number of leading chunks
        for (j = big_int_size_ - 1; j >= (int64_t) big_int_size_ - leading_zero_chunks; --j)
        {
            big_int_array[i]->chunks[j] = 0;
        }
        big_int_array[i]->size = big_int_size_;
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_prune",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_prep(void *argptr)
{
    big_int_size_ = 8;
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_create",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_from_dbl_chunk_prep(void *argptr)
{
    big_int_size_ = 8;
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_create_from_dbl_chunk",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_create_from_hex_prep(void *argptr)
{
    big_int_size_ = 8;
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_create_from_hex",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
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
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, 64);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_create_random",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
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
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_duplicate",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_neg_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_neg_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_neg(big_int_array[i], big_int_array[i]);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_neg",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_abs_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_abs_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_abs(big_int_array[i], big_int_array[i]);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_abs",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_add_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 31);
    }
}

// Benchmark itself
void bench_big_int_add_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_add(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_add",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
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
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 31);
        if (random_q)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_add_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_add_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
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
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_add_mod",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_sub_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 31);
    }
}

// Benchmark itself
void bench_big_int_sub_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_sub(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_sub_cleanup(void)
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

void bench_big_int_sub(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_sub_prep,
        .bench_fn = bench_big_int_sub_fn,
        .bench_cleanup_fn = bench_big_int_sub_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_sub",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_sub_mod_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 31);
        if (random_q)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_sub_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_sub_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
    }
}

// Run after benchmark
void bench_big_int_sub_mod_cleanup(void)
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

void bench_big_int_sub_mod(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_sub_mod_prep,
        .bench_fn = bench_big_int_sub_mod_fn,
        .bench_cleanup_fn = bench_big_int_sub_mod_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_sub_mod",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_mul_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        // needs to be doubled size since no mod
        big_int_array_1[i] = big_int_create_random(NULL, 2 * big_int_size_, 31);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 31);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 31);
    }
}

// Benchmark itself
void bench_big_int_mul_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_mul(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_mul_cleanup(void)
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

void bench_big_int_mul(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_mul_prep,
        .bench_fn = bench_big_int_mul_fn,
        .bench_cleanup_fn = bench_big_int_mul_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_mul",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_mul_mod_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);
        if (random_q)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_mul_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_mul_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
    }
}

// Run after benchmark
void bench_big_int_mul_mod_cleanup(void)
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

void bench_big_int_mul_mod(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_mul_mod_prep,
        .bench_fn = bench_big_int_mul_mod_fn,
        .bench_cleanup_fn = bench_big_int_mul_mod_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_mul_mod",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//This causes Warning: sll caused overflow!
void bench_big_int_div_rem_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_b = ((int *)argptr)[2];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);
        if (random_b)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_div_rem_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_div_rem(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
    }
}

// Run after benchmark
void bench_big_int_div_rem_cleanup(void)
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

void bench_big_int_div_rem(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_div_rem_prep,
        .bench_fn = bench_big_int_div_rem_fn,
        .bench_cleanup_fn = bench_big_int_div_rem_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_div_rem",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//This causes Warning: sll caused overflow!
void bench_big_int_div_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_div_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_div(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_div_cleanup(void)
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

void bench_big_int_div(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_div_prep,
        .bench_fn = bench_big_int_div_fn,
        .bench_cleanup_fn = bench_big_int_div_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_div",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//This causes Warning: sll caused overflow!
void bench_big_int_div_mod_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, 2 * big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);
        if (random_q)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_div_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_div_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
    }
}

// Run after benchmark
void bench_big_int_div_mod_cleanup(void)
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

void bench_big_int_div_mod(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_div_mod_prep,
        .bench_fn = bench_big_int_div_mod_fn,
        .bench_cleanup_fn = bench_big_int_div_mod_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_div_mod",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_sll_small_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    uint64_t_array_1 = (uint64_t *)malloc(big_int_array_size_ * sizeof(uint64_t));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, 2 * big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        uint64_t_array_1[i] = rand() % 256;
    }
}

// Benchmark itself
void bench_big_int_sll_small_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_sll_small(big_int_array_1[i], big_int_array_2[i], uint64_t_array_1[i]);
    }
}

// Run after benchmark
void bench_big_int_sll_small_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
    free(uint64_t_array_1);
}

void bench_big_int_sll_small(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_sll_small_prep,
        .bench_fn = bench_big_int_sll_small_fn,
        .bench_cleanup_fn = bench_big_int_sll_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_sll_small",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//This causes Warning: sll caused overflow!
void bench_big_int_srl_small_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    uint64_t_array_1 = (uint64_t *)malloc(big_int_array_size_ * sizeof(uint64_t));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, 2 * big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        uint64_t_array_1[i] = rand() % 256;
    }
}

// Benchmark itself
void bench_big_int_srl_small_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_srl_small(big_int_array_1[i], big_int_array_2[i], uint64_t_array_1[i]);
    }
}

// Run after benchmark
void bench_big_int_srl_small_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
    free(uint64_t_array_1);
}

void bench_big_int_srl_small(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_srl_small_prep,
        .bench_fn = bench_big_int_srl_small_fn,
        .bench_cleanup_fn = bench_big_int_srl_small_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_srl_small",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_mod_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        if (random_q)
        {
            big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_3[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_mod_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_mod(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_mod_cleanup(void)
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

void bench_big_int_mod(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_mod_prep,
        .bench_fn = bench_big_int_mod_fn,
        .bench_cleanup_fn = bench_big_int_mod_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_mod",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// This leaks memory?
void bench_big_int_inv_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_3[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    }
}

// Benchmark itself
void bench_big_int_inv_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_inv(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_inv_cleanup(void)
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

void bench_big_int_inv(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_inv_prep,
        .bench_fn = bench_big_int_inv_fn,
        .bench_cleanup_fn = bench_big_int_inv_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_inv",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// This leaks memory?
void bench_big_int_pow_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);

        if (random_q)
        {
            big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_4[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_pow_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_pow(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i], big_int_array_4[i]);
    }
}

// Run after benchmark
void bench_big_int_pow_cleanup(void)
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

void bench_big_int_pow(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_pow_prep,
        .bench_fn = bench_big_int_pow_fn,
        .bench_cleanup_fn = bench_big_int_pow_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_pow",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_is_zero_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_is_zero_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_is_zero(big_int_array_1[i]);
    }
}

// Run after benchmark
void bench_big_int_is_zero_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
    }
    free(big_int_array_1);
}

void bench_big_int_is_zero(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_is_zero_prep,
        .bench_fn = bench_big_int_is_zero_fn,
        .bench_cleanup_fn = bench_big_int_is_zero_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_is_zero",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_is_odd_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_is_odd_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_is_odd(big_int_array_1[i]);
    }
}

// Run after benchmark
void bench_big_int_is_odd_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
    }
    free(big_int_array_1);
}

void bench_big_int_is_odd(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_is_odd_prep,
        .bench_fn = bench_big_int_is_odd_fn,
        .bench_cleanup_fn = bench_big_int_is_odd_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_is_odd",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_big_int_compare_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_compare_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_compare(big_int_array_1[i], big_int_array_2[i]);
    }
}

// Run after benchmark
void bench_big_int_compare_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
}

void bench_big_int_compare(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_compare_prep,
        .bench_fn = bench_big_int_compare_fn,
        .bench_cleanup_fn = bench_big_int_compare_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_compare",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//This causes Warning: sll caused overflow!
void bench_big_int_egcd_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_big_int_egcd_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_egcd(big_int_array_1[i], big_int_array_2[i]);
    }
}

// Run after benchmark
void bench_big_int_egcd_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array_1);
    free(big_int_array_2);
}

void bench_big_int_egcd(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_egcd_prep,
        .bench_fn = bench_big_int_egcd_fn,
        .bench_cleanup_fn = bench_big_int_egcd_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_egcd",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// This leaks memory?
void bench_big_int_chi_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];

    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, 64);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, 64);
        if (random_q)
        {
            big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, 64);
        }
        else
        {
            big_int_array_3[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }
    }
}

// Benchmark itself
void bench_big_int_chi_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_chi(big_int_array_1[i], big_int_array_2[i], big_int_array_3[i]);
    }
}

// Run after benchmark
void bench_big_int_chi_cleanup(void)
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

void bench_big_int_chi(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_big_int_chi_prep,
        .bench_fn = bench_big_int_chi_fn,
        .bench_cleanup_fn = bench_big_int_chi_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "big_int_chi",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// This leaks memory?
void bench_elligator_1_string_to_point_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    init_curve1174(&bench_curve);
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    curve_point_array = (CurvePoint *)malloc(big_int_array_size_ * sizeof(CurvePoint));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_create_random(NULL, big_int_size_, 64);
    }
}

// Benchmark itself
void bench_elligator_1_string_to_point_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        curve_point_array[i] = elligator_1_string_to_point(big_int_array[i], bench_curve);
    }
}

// Run after benchmark
void bench_elligator_1_string_to_point_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
    free(curve_point_array);
}

void bench_elligator_1_string_to_point(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_elligator_1_string_to_point_prep,
        .bench_fn = bench_elligator_1_string_to_point_fn,
        .bench_cleanup_fn = bench_elligator_1_string_to_point_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "elligator_1_string_to_point",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// This leaks memory?
void bench_elligator_1_point_to_string_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];

    init_curve1174(&bench_curve);
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    curve_point_array = (CurvePoint *)malloc(big_int_array_size_ * sizeof(CurvePoint));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        curve_point_array[i] = elligator_1_string_to_point(big_int_create_random(NULL, big_int_size_, 64), bench_curve);
    }
}

// Benchmark itself
void bench_elligator_1_point_to_string_fn(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = elligator_1_point_to_string(curve_point_array[i], bench_curve);
    }
}

// Run after benchmark
void bench_elligator_1_point_to_string_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
    free(curve_point_array);
}

void bench_elligator_1_point_to_string(void *bench_args, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_elligator_1_point_to_string_prep,
        .bench_fn = bench_elligator_1_point_to_string_fn,
        .bench_cleanup_fn = bench_elligator_1_point_to_string_cleanup,
    };
    benchmark_runner_always_set_up_and_clean(bench_closure, "elligator_1_point_to_string",
                                             path, SETS, REPS, ((int *)bench_args)[1]);
}



int main(void)
{

    int bench_big_int_size_256_args[] = {8, 100};
    int bench_big_int_size_256_random_mod_args[] = {8, 100, 1};
    int bench_big_int_size_256_curve_mod_args[] = {8, 100, 0};

    int bench_big_int_size_64c_args[] = {64, 100};
    int bench_big_int_size_512c_args[] = {512, 100};
    int bench_big_int_size_4096c_args[] = {4096, 100};


    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_alloc((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_alloc_prep.log");
    bench_big_int_alloc((void *)bench_big_int_size_64c_args, LOG_PATH "/bench_big_int_alloc_prep_64c.log");
    bench_big_int_alloc((void *)bench_big_int_size_512c_args, LOG_PATH "/bench_big_int_alloc_prep_512c.log");
    bench_big_int_alloc((void *)bench_big_int_size_4096c_args, LOG_PATH "/bench_big_int_alloc_prep_4096c.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_calloc((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_calloc_prep.log");
    bench_big_int_calloc((void *)bench_big_int_size_64c_args, LOG_PATH "/bench_big_int_calloc_prep_64c.log");
    bench_big_int_calloc((void *)bench_big_int_size_512c_args, LOG_PATH "/bench_big_int_calloc_prep_512c.log");
    bench_big_int_calloc((void *)bench_big_int_size_4096c_args, LOG_PATH "/bench_big_int_calloc_prep_4096c.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_destroy((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_destroy_prep.log");
    bench_big_int_destroy((void *)bench_big_int_size_64c_args, LOG_PATH "/bench_big_int_destroy_prep_64c.log");
    bench_big_int_destroy((void *)bench_big_int_size_512c_args, LOG_PATH "/bench_big_int_destroy_prep_512c.log");
    bench_big_int_destroy((void *)bench_big_int_size_4096c_args, LOG_PATH "/bench_big_int_destroy_prep_4096c.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_copy((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_copy_prep.log");

    // --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_prune((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_prune_prep.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_create.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_from_dbl_chunk((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_create_from_dbl_chunk.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_from_hex((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_create_from_hex.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_create_random((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_create_random.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_duplicate((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_duplicate.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_neg((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_negate.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_abs((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_abs.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_add((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_add.log");
    bench_big_int_add_mod((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_add_mod_random.log");
    bench_big_int_add_mod((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_add_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_sub((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_sub.log");
    bench_big_int_sub_mod((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_sub_mod_random.log");
    bench_big_int_sub_mod((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_sub_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_mul((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_mul.log");
    bench_big_int_mul_mod((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_mul_mod_random.log");
    bench_big_int_mul_mod((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_mul_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_div_rem((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_div_rem_random.log");
    bench_big_int_div_rem((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_div_rem_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_div((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_div.log");
    bench_big_int_div((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_div_random.log");
    bench_big_int_div((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_div_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_sll_small((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_sll_small.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_srl_small((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_srl_small.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_mod((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_mod_random.log");
    bench_big_int_mod((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_mod_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_inv((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_inv.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_pow((void *)bench_big_int_size_256_random_mod_args, LOG_PATH "/bench_big_int_pow_random.log");
    bench_big_int_pow((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_pow_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_is_zero((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_is_zero.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_is_odd((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_is_odd.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_compare((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_compare.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_egcd((void *)bench_big_int_size_256_args, LOG_PATH "/bench_big_int_egcd.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    bench_big_int_chi((void *)bench_big_int_size_256_curve_mod_args, LOG_PATH "/bench_big_int_chi_curve.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    // TODO: Outsource elligator benchmarks to their own file.
    bench_elligator_1_string_to_point((void *)bench_big_int_size_256_args, LOG_PATH "/bench_elligator_1_string_to_point.log");

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    // bench_elligator_1_point_to_string((void *)bench_big_int_size_256_args, LOG_PATH "/bench_elligator_1_point_to_string.log");

    // BigInt *test = big_int_create_random(NULL, 8, 31);
    // big_int_print(test);
    // printf("\n");
    // test = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    // big_int_print(test);
    // printf("\n");

    return EXIT_SUCCESS;
}

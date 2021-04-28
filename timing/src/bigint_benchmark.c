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

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE 4096
#endif

/*
 * Global BigInt variables
 */
BigInt *i1, *i2, *i3;
BigInt *big_int_array[ARRAY_SIZE];

/**
 * \brief Benchmark the chi function
 * TODO: find appropriate input sizes
*/
// Run before benchmark
void bench_chi_prep(void *argptr)
{
    char **bigint_strings;

    bigint_strings = (char **) argptr;
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
    
}

// Benchmark itself
void bench_big_int_alloc_fn(void)
{
    for(uint64_t i = 0; i < ARRAY_SIZE; i++)
    {
        big_int_array[i] = big_int_alloc(i);
    }
    
}

// Run after benchmark
void bench_big_int_alloc_cleanup(void)
{
    for(uint64_t i = 0; i < ARRAY_SIZE; i++)
    {
       big_int_destroy(big_int_array[i]);
    }
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
void bench_big_int_calloc_prep(void *argptr)
{
    
}

// Benchmark itself
void bench_big_int_calloc_fn(void)
{
    for(uint64_t i = 0; i < ARRAY_SIZE; i++)
    {
        big_int_array[i] = big_int_calloc(i);
    }
    
}

// Run after benchmark
void bench_big_int_calloc_cleanup(void)
{
    for(uint64_t i = 0; i < ARRAY_SIZE; i++)
    {
       big_int_destroy(big_int_array[i]);
    }
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
void bench_big_int_destroy_prep(void *argptr)
{
    for(uint64_t i = 0; i < ARRAY_SIZE; i++)
    {
        big_int_array[i] = big_int_calloc(i);
    }
}

// Benchmark itself
void bench_big_int_destroy_fn(void)
{
    for(uint64_t i = 0; i < ARRAY_SIZE; i++)
    {
       big_int_destroy(big_int_array[i]);
    }
}

// Run after benchmark
void bench_big_int_destroy_cleanup(void)
{
    
}






int main(void)
{
    /*
     * BigInt Chi Benchmark
     */

    // a is no square mod q, i.e., big_int_chi(r, a, q) = -1 mod q
    char *bench_chi_prep_args[] = {
        "0", // r
        "A994F8591AF2F272B64058F", // a
        "68EFDACEEE191D78017E4503", // q
    };

    BenchmarkClosure bench_chi_closure = {
        .bench_prep_args = (void *) bench_chi_prep_args,
        .bench_prep_fn = bench_chi_prep,
        .bench_fn = bench_chi_fn,
        .bench_cleanup_fn = bench_chi_cleanup,
    };

    benchmark_runner(bench_chi_closure, "unopt.",
        LOG_PATH "/bench_chi.log", 50, 1000);

    return EXIT_SUCCESS;
}

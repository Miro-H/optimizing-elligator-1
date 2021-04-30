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
BigInt **big_int_array_2;

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
    big_int_size_ = ((int *) argptr)[0];
    big_int_array_size_ = ((int *) argptr)[1];
    big_int_array = (BigInt**) malloc(big_int_array_size_ * sizeof(BigInt*));
}

// Benchmark itself
void bench_big_int_alloc_fn(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
    }
    
}

// Run after benchmark
void bench_big_int_alloc_cleanup(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
       big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
void bench_big_int_calloc_prep(void *argptr)
{
    big_int_size_ = ((int *) argptr)[0];
    big_int_array_size_ = ((int *) argptr)[1];
    big_int_array = (BigInt**) malloc(big_int_array_size_ * sizeof(BigInt*));
}

// Benchmark itself
void bench_big_int_calloc_fn(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_calloc(big_int_size_);
    }
    
}

// Run after benchmark
void bench_big_int_calloc_cleanup(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
       big_int_destroy(big_int_array[i]);
    }
    free(big_int_array);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
void bench_big_int_destroy_prep(void *argptr)
{
    big_int_size_ = ((int *) argptr)[0];
    big_int_array_size_ = ((int *) argptr)[1];
    big_int_array = (BigInt**) malloc(big_int_array_size_ * sizeof(BigInt*));
    
    
}

// Benchmark itself
void bench_big_int_destroy_fn(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
    }
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
       big_int_destroy(big_int_array[i]);
    }
}

// Run after benchmark
void bench_big_int_destroy_cleanup(void)
{
    free(big_int_array);
}

void bench_big_int_copy_prep(void *argptr)
{
    big_int_size_ = ((int *) argptr)[0];
    big_int_array_size_ = ((int *) argptr)[1];
    big_int_array = (BigInt**) malloc(big_int_array_size_ * sizeof(BigInt*));
    big_int_array_2 = (BigInt**) malloc(big_int_array_size_ * sizeof(BigInt*));
    
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
        big_int_array_2[i] = big_int_alloc(big_int_size_);
    }
}


// Benchmark itself
void bench_big_int_copy_fn(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
        //big_int_copy(big_int_array[i], big_int_array_2[i]); //will throw error right now since we only support fixed size int
    }
    
}

// Run after benchmark
void bench_big_int_copy_cleanup(void)
{
    for(uint64_t i = 0; i < big_int_array_size_; i++)
    {
       big_int_destroy(big_int_array[i]);
       big_int_destroy(big_int_array_2[i]);
    }
    free(big_int_array);
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

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    int bench_big_int_size_4_args[] = {4, 1048576};
    int bench_big_int_size_16_args[] = {16, 1048576};
    int bench_big_int_size_64_args[] = {64, 1048576};
    int bench_big_int_size_256_args[] = {256, 1048576};
    int bench_big_int_size_1024_args[] = {1024, 1048576};


    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


    BenchmarkClosure bench_big_int_alloc_prep_4 = {
        .bench_prep_args = (void *) bench_big_int_size_4_args,
        .bench_prep_fn = bench_big_int_alloc_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_alloc_cleanup,
    };

    BenchmarkClosure bench_big_int_alloc_prep_16 = {
        .bench_prep_args = (void *) bench_big_int_size_16_args,
        .bench_prep_fn = bench_big_int_alloc_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_alloc_cleanup,
    };

    BenchmarkClosure bench_big_int_alloc_prep_64 = {
        .bench_prep_args = (void *) bench_big_int_size_64_args,
        .bench_prep_fn = bench_big_int_alloc_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_alloc_cleanup,
    };

    BenchmarkClosure bench_big_int_alloc_prep_256 = {
        .bench_prep_args = (void *) bench_big_int_size_256_args,
        .bench_prep_fn = bench_big_int_alloc_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_alloc_cleanup,
    };

    BenchmarkClosure bench_big_int_alloc_prep_1024 = {
        .bench_prep_args = (void *) bench_big_int_size_1024_args,
        .bench_prep_fn = bench_big_int_alloc_prep,
        .bench_fn = bench_big_int_alloc_fn,
        .bench_cleanup_fn = bench_big_int_alloc_cleanup,
    };

    benchmark_runner(bench_big_int_alloc_prep_4, "unopt.",
        LOG_PATH "/bench_big_int_alloc_prep_4.log", 1, 1);

    benchmark_runner(bench_big_int_alloc_prep_16, "unopt.",
        LOG_PATH "/bench_big_int_alloc_prep_16.log", 1, 1);
    
    benchmark_runner(bench_big_int_alloc_prep_64, "unopt.",
        LOG_PATH "/bench_big_int_alloc_prep_64.log", 1, 1);
    
    benchmark_runner(bench_big_int_alloc_prep_256, "unopt.",
        LOG_PATH "/bench_big_int_alloc_prep_256.log", 1, 1);
    
    benchmark_runner(bench_big_int_alloc_prep_1024, "unopt.",
        LOG_PATH "/bench_big_int_alloc_prep_1024.log", 1, 1);

    
    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


    BenchmarkClosure bench_big_int_calloc_prep_4 = {
        .bench_prep_args = (void *) bench_big_int_size_4_args,
        .bench_prep_fn = bench_big_int_calloc_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_calloc_cleanup,
    };

    BenchmarkClosure bench_big_int_calloc_prep_16 = {
        .bench_prep_args = (void *) bench_big_int_size_16_args,
        .bench_prep_fn = bench_big_int_calloc_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_calloc_cleanup,
    };

    BenchmarkClosure bench_big_int_calloc_prep_64 = {
        .bench_prep_args = (void *) bench_big_int_size_64_args,
        .bench_prep_fn = bench_big_int_calloc_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_calloc_cleanup,
    };

    BenchmarkClosure bench_big_int_calloc_prep_256 = {
        .bench_prep_args = (void *) bench_big_int_size_256_args,
        .bench_prep_fn = bench_big_int_calloc_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_calloc_cleanup,
    };
    
    BenchmarkClosure bench_big_int_calloc_prep_1024 = {
        .bench_prep_args = (void *) bench_big_int_size_1024_args,
        .bench_prep_fn = bench_big_int_calloc_prep,
        .bench_fn = bench_big_int_calloc_fn,
        .bench_cleanup_fn = bench_big_int_calloc_cleanup,
    };
    
    benchmark_runner(bench_big_int_calloc_prep_4, "unopt.",
        LOG_PATH "/bench_big_int_calloc_prep_4.log", 1, 1);

    benchmark_runner(bench_big_int_calloc_prep_16, "unopt.",
        LOG_PATH "/bench_big_int_calloc_prep_16.log", 1, 1);
    
    benchmark_runner(bench_big_int_calloc_prep_64, "unopt.",
        LOG_PATH "/bench_big_int_calloc_prep_64.log", 1, 1);
    
    benchmark_runner(bench_big_int_calloc_prep_256, "unopt.",
        LOG_PATH "/bench_big_int_calloc_prep_256.log", 1, 1);
    
    
    benchmark_runner(bench_big_int_calloc_prep_1024, "unopt.",
        LOG_PATH "/bench_big_int_calloc_prep_1024.log", 1, 1);
    

    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

    BenchmarkClosure bench_big_int_destroy_prep_4 = {
        .bench_prep_args = (void *) bench_big_int_size_4_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };

    BenchmarkClosure bench_big_int_destroy_prep_16 = {
        .bench_prep_args = (void *) bench_big_int_size_16_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };

    BenchmarkClosure bench_big_int_destroy_prep_64 = {
        .bench_prep_args = (void *) bench_big_int_size_64_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };

    BenchmarkClosure bench_big_int_destroy_prep_256 = {
        .bench_prep_args = (void *) bench_big_int_size_256_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };

    BenchmarkClosure bench_big_int_destroy_prep_1024 = {
        .bench_prep_args = (void *) bench_big_int_size_1024_args,
        .bench_prep_fn = bench_big_int_destroy_prep,
        .bench_fn = bench_big_int_destroy_fn,
        .bench_cleanup_fn = bench_big_int_destroy_cleanup,
    };

    benchmark_runner(bench_big_int_destroy_prep_4, "unopt.",
        LOG_PATH "/bench_big_int_destroy_prep_4.log", 1, 1);

    benchmark_runner(bench_big_int_destroy_prep_16, "unopt.",
        LOG_PATH "/bench_big_int_destroy_prep_16.log", 1, 1);
    
    benchmark_runner(bench_big_int_destroy_prep_64, "unopt.",
        LOG_PATH "/bench_big_int_destroy_prep_64.log", 1, 1);
    
    benchmark_runner(bench_big_int_destroy_prep_256, "unopt.",
        LOG_PATH "/bench_big_int_destroy_prep_256.log", 1, 1);
    
    benchmark_runner(bench_big_int_destroy_prep_1024, "unopt.",
        LOG_PATH "/bench_big_int_destroy_prep_1024.log", 1, 1);


    //--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


    BenchmarkClosure bench_big_int_copy_prep_4 = {
        .bench_prep_args = (void *) bench_big_int_size_4_args,
        .bench_prep_fn = bench_big_int_copy_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_copy_cleanup,
    };

    BenchmarkClosure bench_big_int_copy_prep_16 = {
        .bench_prep_args = (void *) bench_big_int_size_16_args,
        .bench_prep_fn = bench_big_int_copy_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_copy_cleanup,
    };

    BenchmarkClosure bench_big_int_copy_prep_64 = {
        .bench_prep_args = (void *) bench_big_int_size_64_args,
        .bench_prep_fn = bench_big_int_copy_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_copy_cleanup,
    };

    BenchmarkClosure bench_big_int_copy_prep_256 = {
        .bench_prep_args = (void *) bench_big_int_size_256_args,
        .bench_prep_fn = bench_big_int_copy_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_copy_cleanup,
    };

    BenchmarkClosure bench_big_int_copy_prep_1024 = {
        .bench_prep_args = (void *) bench_big_int_size_1024_args,
        .bench_prep_fn = bench_big_int_copy_prep,
        .bench_fn = bench_big_int_copy_fn,
        .bench_cleanup_fn = bench_big_int_copy_cleanup,
    };

    benchmark_runner(bench_big_int_copy_prep_4, "unopt.",
        LOG_PATH "/bench_big_int_copy_prep_4.log", 1, 1);

    benchmark_runner(bench_big_int_copy_prep_16, "unopt.",
        LOG_PATH "/bench_big_int_copy_prep_16.log", 1, 1);
    
    benchmark_runner(bench_big_int_copy_prep_64, "unopt.",
        LOG_PATH "/bench_big_int_copy_prep_64.log", 1, 1);
    
    benchmark_runner(bench_big_int_copy_prep_256, "unopt.",
        LOG_PATH "/bench_big_int_copy_prep_256.log", 1, 1);
    
    benchmark_runner(bench_big_int_copy_prep_1024, "unopt.",
        LOG_PATH "/bench_big_int_copy_prep_1024.log", 1, 1);






    return EXIT_SUCCESS;
}

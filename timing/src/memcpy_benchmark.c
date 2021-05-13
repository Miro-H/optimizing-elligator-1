/*
 * Includes
 */
#include <stdlib.h>

// Include header files
#include "memcpy_benchmark.h"
#include "debug.h"
#include "benchmark_helpers.h"

void bench_memcpy_prep(void *argptr)
{
    int64_t elem_size = ((int64_t *)argptr)[0];
    int64_t array_size = ((int64_t *)argptr)[1];
    mem_array = (void **) malloc(array_size * elem_size);
    mem_elem_size = elem_size;
}

void bench_memcpy_fn(void *argptr)
{
    int64_t i = *((int64_t *) argptr);
    mem_array[i] = (void *) malloc(mem_elem_size);
}

void bench_memcpy_overhead_fn(void *argptr)
{
    int64_t i = *((int64_t *) argptr);
    mem_array[i] = &i;
}

void bench_memcpy_cleanup(void *argptr)
{
    int64_t used_values = ((int64_t *) argptr)[0];

    for (int64_t i = 0; i < used_values; i++)
    {
        free(mem_array[i]);
    }
    free(mem_array);
}

int main(int argc, char const *argv[])
{
    int64_t bench_memcpy_32_args[] = {
        72, // 17 * 4 byte chunks, 4 bytes metadata
        REPS
    };

    BenchmarkClosure bench_memcpy_32_closure = {
        .bench_prep_args = bench_memcpy_32_args,
        .bench_prep_fn = bench_memcpy_prep,
        .bench_fn = bench_memcpy_fn,
        .bench_cleanup_fn = bench_memcpy_cleanup,
    };
    benchmark_runner(bench_memcpy_32_closure, "memcpy 32-bit chunks",
        LOG_PATH "/memcpy_32_bit_chunks.log", SETS, REPS, REPS);


    int64_t bench_memcpy_64_args[] = {
        144, // 17 * 8 byte chunks, 4 bytes metadata, 4 bytes padding
        REPS,
    };

    BenchmarkClosure bench_memcpy_64_closure = {
        .bench_prep_args = bench_memcpy_64_args,
        .bench_prep_fn = bench_memcpy_prep,
        .bench_fn = bench_memcpy_fn,
        .bench_cleanup_fn = bench_memcpy_cleanup,
    };

    benchmark_runner(bench_memcpy_64_closure, "memcpy 64-bit chunks",
        LOG_PATH "/memcpy_64_bit_chunks.log", SETS, REPS, REPS);

    BenchmarkClosure bench_memcpy_overhead_closure = {
        .bench_prep_args = bench_memcpy_64_args,
        .bench_prep_fn = bench_memcpy_prep,
        .bench_fn = bench_memcpy_overhead_fn,
        .bench_cleanup_fn = bench_memcpy_cleanup,
    };

    benchmark_runner(bench_memcpy_overhead_closure, "benchmark overhead",
        LOG_PATH "/memcpy_benchmark_overhead.log", SETS, REPS, 0);


    return EXIT_SUCCESS;
}

/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://github.com/Miro-H/optimizing-elligator-1).
 *
 * Copyright (C) 2021  Christopher Vogelsanger, Freya Murphy, Miro Haller
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact: miro.haller@alumni.ethz.ch
 *
 * Short description of this file:
 * This file implements benchmarks of memcpy.
 */

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

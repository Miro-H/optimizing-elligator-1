/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements helper functions for benchmarking
 */

/*
 * Includes
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

// Include header files
#include "bigint.h"
#include "benchmark_helpers.h"
#include "debug.h"
#include "tsc_x86.h"

/**
 * Local macros
 */
#define HEADER_BAR "----------------------------------------"
#define HEADER_LINE_CHAR_LEN 36
#define HEADER_LINE_BUF_LEN (HEADER_LINE_CHAR_LEN + 6)
#define HEADER_MAX_LEN 4096

/**
 * Local function prototypes
 */
void bench_warmup(BenchmarkClosure bench_closure, uint64_t num_sets,
    uint64_t num_reps, uint64_t used_values);

/**
* \brief Make warmup runs for a benchmark of at least WARMUP_CYCLES cycles.
*/
void bench_warmup(BenchmarkClosure bench_closure, uint64_t num_sets,
    uint64_t num_reps, uint64_t used_values)
{
    uint64_t num_sets_local, cycles, start;
    int64_t i, j;
    uint64_t cleanup_args[2] = {used_values, num_reps};

    cycles = 0;
    num_sets_local = 1;

    do
    {
        cycles = 0;
        for (j = 0; j < num_sets_local; ++j)
        {
            bench_closure.bench_prep_fn(bench_closure.bench_prep_args);
            start = start_tsc();
            for (i = 0; i < num_reps; ++i)
            {
                bench_closure.bench_fn((void *) &i);
            }
            cycles += stop_tsc(start);
        }
        bench_closure.bench_cleanup_fn((void *) cleanup_args);

        num_sets_local <<= 1;
    } while (cycles < WARMUP_CYCLES);

    if (num_sets_local > num_sets) {
        WARNING("Current benchmark uses less than %d cycles. It's recommended "
            "to increase sets * reps to %" PRIu64 "\n",
            WARMUP_CYCLES, num_sets_local * num_reps);
    }
}

/**
 * \brief Run a benchmark
 *
 * \param num_sets Number of times the entire measurement is repeated.
 * \param num_reps Number of calls to the benchmarked function for every set.
 *        If 0, set to the number of reps required to achieve WARMUP_CYCLES
 *        total runtime.
 * \param used_values Values that are newly allocated by the benchmark
*/
void benchmark_runner(BenchmarkClosure bench_closure, char *bench_name,
                      char *log_fname, uint64_t num_sets, uint64_t num_reps,
                      uint64_t used_values)
{
    FILE *out_fp;

    uint64_t cycles, start;
    uint8_t do_write_to_stdout;
    int64_t j;
    uint64_t cleanup_args[2] = {used_values, num_reps};

    printf_bench_header(bench_name);

    do_write_to_stdout = !log_fname;
    if (do_write_to_stdout)
        out_fp = stdout;
    else
        out_fp = fopen(log_fname, "w+");

    assert(out_fp);
    fprintf(out_fp, "%s\n", bench_name);

    // For collecting stats, we have no warmup instructions and only one call
    // of the benchmarked function.
#ifdef COLLECT_STATS
    fprintf(out_fp, "Function name, Function calls\n");
    cleanup_args[0] = (used_values) ? 1 : 0;

    bench_closure.bench_prep_fn(bench_closure.bench_prep_args);
    reset_stats();

    j = 0;
    start = start_tsc();
    bench_closure.bench_fn((void *) &j);
    cycles = stop_tsc(start);

    for (j = 0; j < BIGINT_TYPE_LAST; ++j)
    {
        fprintf(out_fp, "%s, %" PRIu64 "\n", big_int_type_names[j], big_int_stats[j]);
        fflush(out_fp);
    }

    bench_closure.bench_cleanup_fn((void *) cleanup_args);
#else
    fprintf(out_fp, "Measurement, Runtime [cycles]\n");
    bench_warmup(bench_closure, num_sets, num_reps, used_values);

    for (uint64_t i = 0; i < num_sets; ++i)
    {
        bench_closure.bench_prep_fn(bench_closure.bench_prep_args);

        start = start_tsc();
        for (j = 0; j < num_reps; ++j)
        {
            bench_closure.bench_fn((void *) &j);
        }
        cycles = stop_tsc(start);

        fprintf(out_fp, "%" PRId64 ", %.02lf\n", i, (double)cycles / num_reps);
        fflush(out_fp);

        bench_closure.bench_cleanup_fn((void *) cleanup_args);
    }
#endif

    if (!do_write_to_stdout)
    {
        printf("Wrote benchmark results to log file: '%s'\n", log_fname);
        fclose(out_fp);
    }
}

/*
 * Fancy benchmark printing
 */
void printf_bench_header(const char *fmt, ...)
{
    va_list argptr;
    char header[HEADER_MAX_LEN];
    char line[HEADER_LINE_BUF_LEN];
    int len, left_pad, right_pad, pad_len;

    printf("%s\n", HEADER_BAR);

    va_start(argptr, fmt);
    len = vsnprintf(header, HEADER_MAX_LEN, fmt, argptr);
    va_end(argptr);

    left_pad = right_pad = 0;

    for (size_t i = 0; i < len; i += HEADER_LINE_CHAR_LEN)
    {
        if (len - i < HEADER_LINE_CHAR_LEN)
        {
            pad_len = HEADER_LINE_CHAR_LEN - (len - i);
            left_pad = pad_len / 2;
            right_pad = pad_len - left_pad;
        }

        snprintf(line, HEADER_LINE_CHAR_LEN + 1, "%s", header + i);
        printf("- %*s%s%*s -\n", left_pad, "", line, right_pad, "");
    }
    printf("%s\n", HEADER_BAR);
}

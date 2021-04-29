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

// Include header files
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
uint64_t bench_warmup(BenchmarkClosure bench_closure);


/**
* \brief Make warmup runs for a benchmark of at least WARMUP_CYCLES cycles.
*/
uint64_t bench_warmup(BenchmarkClosure bench_closure)
{
    uint64_t num_reps, cycles, start;
    int i;

    num_reps = 1;
    cycles = 0;

    bench_closure.bench_prep_fn(bench_closure.bench_prep_args);
    do {
        num_reps <<= 1;
        start = start_tsc();
        for (i = 0; i < num_reps; ++i) {
            bench_closure.bench_fn();
        }
        cycles = stop_tsc(start);
    } while(cycles < WARMUP_CYCLES);

    bench_closure.bench_cleanup_fn();

    return num_reps;
}

/**
 * \brief Run a benchmark
 *
 * \param num_sets Number of times the entire measurement is repeated.
 * \param num_reps Number of calls to the benchmarked function for every set.
 *        If 0, set to the number of reps required to achieve WARMUP_CYCLES
 *        total runtime.
*/
void benchmark_runner(BenchmarkClosure bench_closure, char *bench_name,
    char *log_fname, uint64_t num_sets, uint64_t num_reps)
{
    FILE *out_fp;
    uint64_t num_reps_warmup, cycles, start;
    uint8_t do_write_to_stdout;
    int64_t i, j;

    printf_bench_header(bench_name);

    do_write_to_stdout = !log_fname;
    if (do_write_to_stdout)
        out_fp = stdout;
    else
        out_fp = fopen(log_fname, "w+");

    assert(out_fp);

    fprintf(out_fp, "%s\n", bench_name);
    fprintf(out_fp, "Measurement, Runtime [cycles]\n");

    num_reps_warmup = bench_warmup(bench_closure);
    if (!num_reps)
        num_reps = num_reps_warmup;

    for (j = 0; j < num_sets; ++j) {
        bench_closure.bench_prep_fn(bench_closure.bench_prep_args);

        start = start_tsc();
        for (i = 0; i < num_reps; ++i) {
            bench_closure.bench_fn();
        }
        cycles = stop_tsc(start);

        fprintf(out_fp, "%" PRId64 ", %.02lf\n", j, (double) cycles / num_reps);
        bench_closure.bench_cleanup_fn();
    }

    if (!do_write_to_stdout)
        printf("Wrote benchmark results to log file: '%s'\n", log_fname);
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

    for (size_t i = 0; i < len; i += HEADER_LINE_CHAR_LEN) {
        if (len - i < HEADER_LINE_CHAR_LEN) {
            pad_len = HEADER_LINE_CHAR_LEN - (len - i);
            left_pad = pad_len/2;
            right_pad = pad_len - left_pad;
        }

        snprintf(line, HEADER_LINE_CHAR_LEN + 1, "%s", header + i);
        printf("- %*s%s%*s -\n", left_pad, "", line, right_pad, "");
    }
    printf("%s\n", HEADER_BAR);
}

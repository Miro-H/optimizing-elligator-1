/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements benchmarks of the GNU MP Bignum Library.
 */

/*
 * Includes
 */
#include <gmp.h>
#include <stdlib.h>

// Include header files
#include "benchmark_helpers.h"

#ifndef LOG_PATH
    #define LOG_PATH "../logs"
#endif

int main(void)
{
    printf_bench_header("GMP Benchmarks");
    return EXIT_SUCCESS;
}

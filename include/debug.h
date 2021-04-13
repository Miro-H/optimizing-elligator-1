/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * Debug header file defining useful macros for debugging.
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>
#include <assert.h>
#include "bigint.h"

// Set the log level to receive more output. Available levels are:
// 0: quiet; no output
// 1: fatal; only get fatal errors
// 2: error; additionally, get non-fatal errors
// 3: warnings; additionally, get warnings
// 4: success; additionally, print successful statements
// 5: vebose; get general debug output
#define LOG_LEVEL 5

// Terminal colored print statements
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define PREFIX_FATAL "Fatal: "
#define PREFIX_ERROR "Error: "
#define PREFIX_WARNING "Warning: "
#define PREFIX_SUCCESS "Success: "
#define PREFIX_DEBUG "Debug: "

__attribute__((unused)) static const char *prefix[] = {
    ANSI_COLOR_RED PREFIX_FATAL,
    ANSI_COLOR_MAGENTA PREFIX_ERROR,
    ANSI_COLOR_YELLOW PREFIX_WARNING,
    ANSI_COLOR_GREEN PREFIX_SUCCESS,
    ANSI_COLOR_BLUE PREFIX_DEBUG,
};

#define OUTFILE(lvl) (((lvl) <= 3) ? stderr : stdout)

#define LOG_PRINTF(lvl, fmt...)                         \
    if (lvl <= LOG_LEVEL) {                             \
        fprintf(OUTFILE(lvl), "%s", prefix[lvl-1]);     \
        fprintf(OUTFILE(lvl), fmt);                     \
        fprintf(OUTFILE(lvl), ANSI_COLOR_RESET);        \
        fflush(OUTFILE(lvl));                           \
        if (lvl == 1)                                   \
            assert(0);                                  \
    }

#define FATAL(fmt...) LOG_PRINTF(1, fmt)
#define ERROR(fmt...) LOG_PRINTF(2, fmt)
#define WARNING(fmt...) LOG_PRINTF(3, fmt)
#define SUCCESS(fmt...) LOG_PRINTF(4, fmt)
#define DEBUG(fmt...) LOG_PRINTF(5, fmt)

#define DEBUG_BIGINT(bigint, fmt...)            \
    do {                                        \
        printf(ANSI_COLOR_BLUE PREFIX_DEBUG);   \
        printf(fmt);                            \
        big_int_print((bigint));                \
        printf("\n");                           \
        printf(ANSI_COLOR_RESET);               \
        fflush(stdout);                         \
    } while(0);

#endif // DEBUG_H_

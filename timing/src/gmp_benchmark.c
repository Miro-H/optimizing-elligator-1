/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements benchmarks of the GNU MP Bignum Library (GMP).
 */

/*
 * Includes
 */
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

// Include header files
#include "benchmark_helpers.h"
#include "gmp_benchmark.h"
#include "benchmark_types.h"
#include "debug.h"

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief Prep function to initialise arrays before running a benchmark
 */
void bench_GMP_prep(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_1 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_2 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_3 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_4 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_5 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_q = (mpz_t *)malloc(array_size * sizeof(mpz_t));

    int8_t_array_1 = (int8_t *)malloc(array_size * sizeof(int8_t));
    uint64_t_array_1 = (uint64_t *)malloc(array_size * sizeof(uint64_t));

    gmp_randstate_t state;
    gmp_randinit_default(state);

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array_1[i]);
        mpz_urandomb(mpz_array_1[i], state, 256);

        mpz_init(mpz_array_2[i]);
        mpz_urandomb(mpz_array_2[i], state, 256);

        mpz_init(mpz_array_3[i]);
        mpz_urandomb(mpz_array_3[i], state, 256);

        mpz_init(mpz_array_4[i]);
        mpz_urandomb(mpz_array_4[i], state, 256);

        mpz_init(mpz_array_5[i]);
        mpz_urandomb(mpz_array_5[i], state, 256);

        if (random_q)
        {
            mpz_init(mpz_array_q[i]);
            mpz_urandomb(mpz_array_q[i], state, 256);
        }
        else
        {
            mpz_init(mpz_array_q[i]);
            mpz_set_str(mpz_array_q[i],
                "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7",
                16);
        }

        uint64_t_array_1[i] = rand() % 256;
    }
}

/**
 * \brief Prep function that initialises arrays and calls the mpz_init function
 *        on mpz_array.
 */
void bench_GMP_prep_with_init(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_1 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_2 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_3 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_4 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_5 = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_q = (mpz_t *)malloc(array_size * sizeof(mpz_t));

    int8_t_array_1 = (int8_t *)malloc(array_size * sizeof(int8_t));
    uint64_t_array_1 = (uint64_t *)malloc(array_size * sizeof(uint64_t));

    gmp_randstate_t state;
    gmp_randinit_default(state);

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array[i]);

        mpz_init(mpz_array_1[i]);
        mpz_urandomb(mpz_array_1[i], state, 256);

        mpz_init(mpz_array_2[i]);
        mpz_urandomb(mpz_array_2[i], state, 256);

        mpz_init(mpz_array_3[i]);
        mpz_urandomb(mpz_array_3[i], state, 256);

        mpz_init(mpz_array_4[i]);
        mpz_urandomb(mpz_array_4[i], state, 256);

        mpz_init(mpz_array_5[i]);
        mpz_urandomb(mpz_array_5[i], state, 256);

        if (random_q)
        {
            mpz_init(mpz_array_q[i]);
            mpz_urandomb(mpz_array_4[i], state, 256);
        }
        else
        {
            mpz_init(mpz_array_q[i]);
            mpz_set_str(mpz_array_q[i],
                "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7",
                16);
        }

        uint64_t_array_1[i] = rand() % 256;
    }
}

/**
 * \brief Cleanup function to free arrays after running a benchmark.
 *        Shoulf be used with bench_GMP_prep and bench_GMP_prep_with_init
 */
void bench_GMP_cleanup(void *argptr)
{
    int64_t used_values = ((int64_t *) argptr)[0];
    int64_t array_size = ((int64_t *) argptr)[1];

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_clear(mpz_array_1[i]);
        mpz_clear(mpz_array_2[i]);
        mpz_clear(mpz_array_3[i]);
        mpz_clear(mpz_array_4[i]);
        mpz_clear(mpz_array_5[i]);
        mpz_clear(mpz_array_q[i]);
    }

    for (uint64_t i = 0; i < used_values; i++)
    {
        mpz_clear(mpz_array[i]);
    }
    free(mpz_array);
    free(mpz_array_1);
    free(mpz_array_2);
    free(mpz_array_3);
    free(mpz_array_4);
    free(mpz_array_5);
    free(mpz_array_q);

    free(int8_t_array_1);
    free(uint64_t_array_1);
}

/**
 * \brief Initialises mpz_array and mpz_array_1 for use in a benchmark.
 */
void bench_GMP_small_prep(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_1 = (mpz_t *)malloc(array_size * sizeof(mpz_t));

    gmp_randstate_t state;
    gmp_randinit_default(state);

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array_1[i]);
        mpz_urandomb(mpz_array_1[i], state, 256);
    }
}

/**
 * \brief Initialises mpz_array and mpz_array_1 for use in a benchmark,
 *        and calls mpz_init on mpz_array.
 */
void bench_GMP_small_prep_with_init(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    mpz_array_1 = (mpz_t *)malloc(array_size * sizeof(mpz_t));

    gmp_randstate_t state;
    gmp_randinit_default(state);

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array_1[i]);
        mpz_urandomb(mpz_array_1[i], state, 256);

        mpz_init(mpz_array[i]);
    }
}

/**
 * \brief Cleanup function to free arrays after running a benchmark.
 *        Should be used with bench_GMP_small_prep and
 *        bench_GMP__small_prep_with_init.
 */
void bench_GMP_small_cleanup(void *argptr)
{
    int64_t used_values = ((int64_t *) argptr)[0];
    int64_t array_size = ((int64_t *) argptr)[1];

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_clear(mpz_array_1[i]);
    }

    for (uint64_t i = 0; i < used_values; i++)
    {
        mpz_clear(mpz_array[i]);
    }
    free(mpz_array);
    free(mpz_array_1);
}

/**
 * \brief Prep function to initialise arrays for the mpz_clear benchmark.
 */
void bench_mpz_clear_prep(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t));
    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array[i]);
    }
}

/**
 * \brief Cleanup function to use after running mpz_clear benchmark.
 */
void bench_mpz_clear_cleanup(void *argptr)
{
    free(mpz_array);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_init is similar to big_int_alloc. It must be called before
 *        the variable can be used.
 */
void bench_mpz_init_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_init(mpz_array[i]);
}

void bench_mpz_init(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_small_prep,
        .bench_fn = bench_mpz_init_fn,
        .bench_cleanup_fn = bench_GMP_small_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, REPS);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_clear is similar to big_int_destroy. It must be called
 *        when you no longer need a variable.
 */
void bench_mpz_clear_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_clear(mpz_array[i]);
}

void bench_mpz_clear(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_mpz_clear_prep,
        .bench_fn = bench_mpz_clear_fn,
        .bench_cleanup_fn = bench_mpz_clear_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_set is similar to big_int_copy. It sets the value of one mpz_t
 *        variable to that of another mpz_t variable.
 */

void bench_mpz_set_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_set(mpz_array_1[i], mpz_array_2[i]);
}

void bench_mpz_set(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_set_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_set_si is similar to big_int_create. It sets the value of an
 *        mpz_t variable to a signed int value.
 */
void bench_mpz_set_si_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_set_si(mpz_array[i], 1);
}

void bench_mpz_set_si(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_small_prep_with_init,
        .bench_fn = bench_mpz_set_si_fn,
        .bench_cleanup_fn = bench_GMP_small_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, REPS);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_set_str is similar to big_int_create. It sets the value of an
 *        mpz_t variable to a string value in a specified base.
 */
void bench_mpz_set_str_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_set_str(mpz_array[i],
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
        16);
}

void bench_mpz_set_str(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep_with_init,
        .bench_fn = bench_mpz_set_str_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, REPS);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_urandomb is similar to big_int_create_random. It sets the value
 *        of an mpz_t variable to a random integer between 0 and 2^b.
 */
void bench_mpz_urandomb_fn(void *arg)
{
    gmp_randstate_t state;
    gmp_randinit_default(state);
    int64_t i = *((int64_t *) arg);
    mpz_urandomb(mpz_array[i], state, 256);
}

void bench_mpz_urandomb(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_small_prep_with_init,
        .bench_fn = bench_mpz_urandomb_fn,
        .bench_cleanup_fn = bench_GMP_small_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, REPS);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief GMP doesn't have a duplicate function so here we call mpz_init()
 *        on a new variable and mpz_set to give the same functionality.
 */
void bench_mpz_duplicate_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_init(mpz_array[i]);
    mpz_set(mpz_array[i], mpz_array_1[i]);
}

void bench_mpz_duplicate(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_small_prep,
        .bench_fn = bench_mpz_duplicate_fn,
        .bench_cleanup_fn = bench_GMP_small_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, REPS);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_neg is analagous to the big_int_neg function.
 */
void bench_mpz_neg_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_neg(mpz_array_1[i], mpz_array_1[i]);
}

void bench_mpz_neg(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_neg_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_abs is analagous to the big_int_abs function.
 */
void bench_mpz_abs_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_abs(mpz_array_1[i], mpz_array_1[i]);
}

void bench_mpz_abs(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_abs_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_add is analagous to the big_int_add function.
 */
void bench_mpz_add_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_add(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
}

void bench_mpz_add(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_add_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief There is no built-in add_mod function in GMP so here we simply
 *        perform an add followed by a mod.
 */
void bench_mpz_add_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_add(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
    mpz_mod(mpz_array_1[i], mpz_array_1[i], mpz_array_q[i]);
}

void bench_mpz_add_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_add_mod_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_sub is analagous to the big_int_sub function.
 */
void bench_mpz_sub_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_sub(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
}

void bench_mpz_sub(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_sub_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief There is no built-in sub_mod function in GMP so here we simply
 *        perform a sub followed by a mod.
 */
void bench_mpz_sub_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_sub(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
    mpz_mod(mpz_array_1[i], mpz_array_1[i], mpz_array_q[i]);
}

void bench_mpz_sub_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_sub_mod_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_mul is analagous to the big_int_mul function.
 */
void bench_mpz_mul_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_mul(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
}

void bench_mpz_mul(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_mul_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief There is no built-in mul_mod function in GMP so here we simply
 *        perform a mul followed by a mod.
 */
void bench_mpz_mul_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_mul(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
    mpz_mod(mpz_array_1[i], mpz_array_1[i], mpz_array_1[i]);
}

void bench_mpz_mul_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_mul_mod_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_tdiv_qr is similar to the big_int_div_rem function.
 */
void bench_mpz_tdiv_qr_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_tdiv_qr(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i],
        mpz_array_q[i]);
}

void bench_mpz_tdiv_qr(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_tdiv_qr_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_tdiv_q is similar to the big_int_div function.
 */
void bench_mpz_tdiv_q_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_tdiv_q(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
}

void bench_mpz_tdiv_q(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_tdiv_q_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief There is no built-in div_mod function in GMP so here we simply
 *        perform a div and then a mod.
 */
void bench_mpz_tdiv_q_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_tdiv_q(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i]);
    mpz_mod(mpz_array_1[i], mpz_array_1[i], mpz_array_q[i]);
}

void bench_mpz_tdiv_q_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_tdiv_q_mod_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_mul_2exp is similar to the big_int_sll_small function.
 */
void bench_mpz_mul_2exp_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_mul_2exp(mpz_array_1[i], mpz_array_2[i], uint64_t_array_1[i]);
}

void bench_mpz_mul_2exp(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_mul_2exp_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_tdiv_q_2exp is similar to the big_int_srl_small function.
 */
void bench_mpz_tdiv_q_2exp_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_tdiv_q_2exp(mpz_array_1[i], mpz_array_2[i], uint64_t_array_1[i]);
}

void bench_mpz_tdiv_q_2exp(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_tdiv_q_2exp_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}


//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_mod is analagous to the big_int_mod function.
 */
void bench_mpz_mod_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_mod(mpz_array_1[i], mpz_array_2[i], mpz_array_q[i]);
}

void bench_mpz_mod(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_mod_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_invert is analagous to the big_int_inv function.
 */
void bench_mpz_invert_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_invert(mpz_array_1[i], mpz_array_2[i], mpz_array_q[i]);
}

void bench_mpz_invert(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_invert_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_powm is analagous to the big_int_pow function.
 */
void bench_mpz_powm_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_powm(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i],
        mpz_array_q[i]);
}

void bench_mpz_powm(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_powm_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief There is no is_zero function in GMP. Instead we use mpz_cmp_si
 *        function to compare an mpz_t value to 0.
 */
void bench_mpz_cmp_si_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_cmp_si(mpz_array_1[i], 0);
}

void bench_mpz_cmp_si(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_cmp_si_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}


//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief There is no is_odd function in GMP. Instead we use the
 *        mpz_divisible_2exp_p function which checks whether a value is
 *        divisible by a power of 2. We use 2^1 to simply check whether the
 *        value is even.
 */
void bench_mpz_divisible_2exp_p_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_divisible_2exp_p(mpz_array_1[i], 1);
}

void bench_mpz_divisible_2exp_p(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_divisible_2exp_p_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_cmp is analagous to the big_int_compare function.
 */
void bench_mpz_cmp_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    int8_t_array_1[i] = mpz_cmp(mpz_array_1[i], mpz_array_2[i]);
}

void bench_mpz_cmp(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_cmp_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief mpz_gcdext is similar to the big_int_egcd function.
 */
void bench_mpz_gcdext_fn(void *arg)
{
    int64_t i = *((int64_t *) arg);
    mpz_gcdext(mpz_array_1[i], mpz_array_2[i], mpz_array_3[i],
        mpz_array_4[i], mpz_array_5[i]);
}

void bench_mpz_gcdext(void *bench_args, char *bench_name, char *path)
{
    BenchmarkClosure bench_closure = {
        .bench_prep_args = bench_args,
        .bench_prep_fn = bench_GMP_prep,
        .bench_fn = bench_mpz_gcdext_fn,
        .bench_cleanup_fn = bench_GMP_cleanup,
    };
    benchmark_runner(bench_closure, bench_name, path, SETS, REPS, 0);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/**
 * \brief Create and run the benchmarks.
 */
int main(int argc, char const *argv[])
{
    int bench_type;
    for (int i = 1; i < argc; ++i) {
        bench_type = atoi(argv[i]);

        // init on MPZ should correspond to BigInt alloc
        BENCHMARK(bench_type, BENCH_TYPE_ALLOC,
            bench_mpz_init((void *)bench_mpz_size_256_args, "alloc",
                LOG_PATH "/gmp_mpz_init.log"));

        // clear on MPZ should correspond to BigInt destroy
        BENCHMARK(bench_type, BENCH_TYPE_DESTROY,
            bench_mpz_clear((void *)bench_mpz_size_256_args, "destroy",
                LOG_PATH "/gmp_mpz_clear.log"));

        // set on MPZ should correspond to BigInt copy
        BENCHMARK(bench_type, BENCH_TYPE_COPY,
            bench_mpz_set((void *)bench_mpz_size_256_args, "copy",
                LOG_PATH "/gmp_mpz_set.log"));

        BENCHMARK(bench_type, BENCH_TYPE_CREATE,
            bench_mpz_set_si((void*)bench_mpz_size_256_args,
                "set from signed int", LOG_PATH "/gmp_mpz_set_si.log"));

        // "set from string" on MPZ should roughly correspond to BigInt "create from hex"
        BENCHMARK(bench_type, BENCH_TYPE_CREATE_HEX,
            bench_mpz_set_str((void *)bench_mpz_size_256_args,
                "create from hex", LOG_PATH "/gmp_mpz_set_str.log"));

        // urandomb from MPZ should roughly correspond to BigInt create random
        BENCHMARK(bench_type, BENCH_TYPE_CREATE_RANDOM,
            bench_mpz_urandomb((void *)bench_mpz_size_256_args,
                "create random", LOG_PATH "/gmp_mpz_urandomb.log"));

        // duplicate from MPZ should correspond to BigInt duplicate
        BENCHMARK(bench_type, BENCH_TYPE_DUPLICATE,
            bench_mpz_duplicate((void *)bench_mpz_size_256_args, "duplicate",
                LOG_PATH "/gmp_mpz_duplicate.log"));

        // neg from MPZ should correspond to BigInt negation
        BENCHMARK(bench_type, BENCH_TYPE_NEG,
            bench_mpz_neg((void *)bench_mpz_size_256_args,
                "negate", LOG_PATH "/gmp_mpz_neg.log"));

        // abs from MPZ should correspond to BigInt abs
        BENCHMARK(bench_type, BENCH_TYPE_ABS,
            bench_mpz_abs((void *)bench_mpz_size_256_args,
                "abs", LOG_PATH "/gmp_mpz_abs.log"));

        // add from MPZ should correspond to BigInt add
        BENCHMARK(bench_type, BENCH_TYPE_ADD,
            bench_mpz_add((void *)bench_mpz_size_256_args,
                "add", LOG_PATH "/gmp_mpz_add.log"));

        // add from MPZ should correspond to BigInt add
        BENCHMARK(bench_type, BENCH_TYPE_ADD_MOD_CURVE,
            bench_mpz_add_mod((void *)bench_mpz_size_256_curve_mod_args,
                "add mod (curve)",
                LOG_PATH "/gmp_mpz_add_mod_curve.log"));

        // add from MPZ should correspond to BigInt add
        BENCHMARK(bench_type, BENCH_TYPE_ADD_MOD_RANDOM,
            bench_mpz_add_mod((void *)bench_mpz_size_256_random_mod_args,
                "add mod (random)",
                    LOG_PATH "/gmp_mpz_add_mod_random.log"));

        // sub from MPZ should correspond to BigInt sub
        BENCHMARK(bench_type, BENCH_TYPE_SUB,
            bench_mpz_sub((void *)bench_mpz_size_256_args,
                "sub", LOG_PATH "/gmp_mpz_sub.log"));

        // sub from MPZ should correspond to BigInt sub
        BENCHMARK(bench_type, BENCH_TYPE_SUB_MOD_CURVE,
            bench_mpz_sub_mod((void *)bench_mpz_size_256_curve_mod_args,
                "sub mod (curve)", LOG_PATH "/gmp_mpz_sub_mod_curve.log"));

        // sub from MPZ should correspond to BigInt sub
        BENCHMARK(bench_type, BENCH_TYPE_SUB_MOD_RANDOM,
            bench_mpz_sub_mod((void *)bench_mpz_size_256_random_mod_args,
                "sub mod (random)", LOG_PATH "/gmp_mpz_sub_mod_random.log"));

        // mul from MPZ should correspond to BigInt mul
        BENCHMARK(bench_type, BENCH_TYPE_MUL,
            bench_mpz_mul((void *)bench_mpz_size_256_args,
                "mul", LOG_PATH "/gmp_mpz_mul.log"));

        // mul from MPZ should correspond to BigInt mul
        BENCHMARK(bench_type, BENCH_TYPE_MUL_CURVE,
            bench_mpz_mul_mod((void *)bench_mpz_size_256_curve_mod_args,
                "mul mod (curve)", LOG_PATH "/gmp_mpz_mul_mod_curve.log"));

        // mul from MPZ should correspond to BigInt mul
        BENCHMARK(bench_type, BENCH_TYPE_MUL_MOD_RANDOM,
            bench_mpz_mul_mod((void *)bench_mpz_size_256_random_mod_args,
                "mul mod (random)", LOG_PATH "/gmp_mpz_mul_mod_random.log"));

        // tdiv_qr from MPZ should correspond to BigInt divrem
        BENCHMARK(bench_type, BENCH_TYPE_DIVREM,
            bench_mpz_tdiv_qr((void *)bench_mpz_size_256_random_mod_args,
                "divrem", LOG_PATH "/gmp_mpz_tdiv_qr.log"));

        // tdiv_qr from MPZ should correspond to BigInt divrem
        BENCHMARK(bench_type, BENCH_TYPE_DIVREM_CURVE,
            bench_mpz_tdiv_qr((void *)bench_mpz_size_256_curve_mod_args,
                "divrem (curve)", LOG_PATH "/gmp_mpz_tdiv_qr_curve.log"));

        // tdiv_qr from MPZ should correspond to BigInt divrem
        BENCHMARK(bench_type, BENCH_TYPE_DIVREM_RANDOM,
            bench_mpz_tdiv_qr((void *)bench_mpz_size_256_random_mod_args,
                "divrem (random)", LOG_PATH "/gmp_mpz_tdiv_qr_random.log"));

        // tdiv_q from MPZ should correspond to BigInt div
        BENCHMARK(bench_type, BENCH_TYPE_DIV,
            bench_mpz_tdiv_q((void *)bench_mpz_size_256_args,
                "div", LOG_PATH "/gmp_mpz_tdiv_q.log"));

        // tdiv_q from MPZ should correspond to BigInt div
        BENCHMARK(bench_type, BENCH_TYPE_DIV_CURVE,
            bench_mpz_tdiv_q_mod((void *)bench_mpz_size_256_curve_mod_args,
                "div (curve)", LOG_PATH "/gmp_mpz_tdiv_q_mod_curve.log"));

        // tdiv_q from MPZ should correspond to BigInt div
        BENCHMARK(bench_type, BENCH_TYPE_DIV_RANDOM,
            bench_mpz_tdiv_q_mod((void *)bench_mpz_size_256_random_mod_args,
                "div (random)", LOG_PATH "/gmp_mpz_tdiv_q_mod_random.log"));

        // mul_2exp from MPZ should correspond to BigInt sll
        BENCHMARK(bench_type, BENCH_TYPE_SLL,
            bench_mpz_mul_2exp((void *)bench_mpz_size_256_args,
                "sll", LOG_PATH "/gmp_mpz_mul_2exp.log"));

        // tdiv_q_2exp from MPZ should correspond to BigInt srl
        BENCHMARK(bench_type, BENCH_TYPE_SRL,
            bench_mpz_tdiv_q_2exp((void *)bench_mpz_size_256_args,
                "srl", LOG_PATH "/gmp_mpz_tdiv_q_2exp.log"));

        // mod from MPZ should correspond to BigInt mod
        BENCHMARK(bench_type, BENCH_TYPE_MOD_CURVE,
            bench_mpz_mod((void *)bench_mpz_size_256_curve_mod_args,
                "mod (curve)", LOG_PATH "/gmp_mpz_mod_curve.log"));

        // mod from MPZ should correspond to BigInt mod
        BENCHMARK(bench_type, BENCH_TYPE_MOD_RANDOM,
            bench_mpz_mod((void *)bench_mpz_size_256_random_mod_args,
                "mod (random)", LOG_PATH "/gmp_mpz_mod_random.log"));

        // invert from MPZ should correspond to BigInt inv
        BENCHMARK(bench_type, BENCH_TYPE_INV,
            bench_mpz_invert((void *)bench_mpz_size_256_args,
                "inv", LOG_PATH "/gmp_mpz_invert.log"));

        // powm from MPZ should correspond to BigInt pow
        BENCHMARK(bench_type, BENCH_TYPE_POW_CURVE,
            bench_mpz_powm((void *)bench_mpz_size_256_curve_mod_args,
            "pow (curve)", LOG_PATH "/gmp_mpz_powm_curve.log"));

        // powm from MPZ should correspond to BigInt pow
        BENCHMARK(bench_type, BENCH_TYPE_POW_RANDOM,
            bench_mpz_powm((void *)bench_mpz_size_256_random_mod_args,
                "pow (random)", LOG_PATH "/gmp_mpz_powm_random.log"));

        // cmp_si from MPZ should correspond to BigInt is_zero
        BENCHMARK(bench_type, BENCH_TYPE_IS_ODD,
            bench_mpz_cmp_si((void *)bench_mpz_size_256_args,
                "check if zero", LOG_PATH "/gmp_mpz_cmp_si.log"));

        // divisible_2exp_p benchmark should roughly correspond to BigInt is_odd
        BENCHMARK(bench_type, BENCH_TYPE_IS_ODD,
            bench_mpz_divisible_2exp_p((void *)bench_mpz_size_256_args,
                "check if odd", LOG_PATH "/gmp_mpz_divisible_2exp_p.log"));

        // cmp from MPZ should correspond to BigInt cmp
        BENCHMARK(bench_type, BENCH_TYPE_COMPARE,
            bench_mpz_cmp((void *)bench_mpz_size_256_args,
                "compare", LOG_PATH "/gmp_mpz_cmp.log"));

        // gcdext from MPZ should correspond to BigInt egcd
        BENCHMARK(bench_type, BENCH_TYPE_EGCD,
            bench_mpz_gcdext((void *)bench_mpz_size_256_args,
                "eGCD", LOG_PATH "/gmp_mpz_gcdext.log"));
    }

    return EXIT_SUCCESS;
}

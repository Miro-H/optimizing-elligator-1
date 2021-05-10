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

// Include header files
#include "benchmark_helpers.h"
#include "runtime_benchmark.h"
#include "benchmark_types.h"

/*
 * Global mpz_t variables
 */
mpz_t *mpz_array;
mpz_t *mpz_array_1;
mpz_t *mpz_array_2;
mpz_t *mpz_array_3;
mpz_t *mpz_array_4;
mpz_t *mpz_array_q;

int8_t *int8_t_array_1;
uint64_t *uint64_t_array_1;

int mpz_size_;

int bench_mpz_size_256_args[] = {BIGINTSIZE, REPS, 1};

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

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

        if (random_q)
        {
            mpz_init(mpz_array_q[i]);
            mpz_urandomb(mpz_array_4[i], state, 256);
        }
        else
        {
            mpz_init(mpz_array_q[i]);
            mpz_set_str(mpz_array_q[i], "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7", 16);
        }

        uint64_t_array_1[i] = rand() % 256;
    }
}

// Run after benchmark
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
    free(mpz_array_q);

    free(int8_t_array_1);
    free(uint64_t_array_1);
}

void bench_GMP_small_prep(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t *));
    mpz_array_1 = (mpz_t *)malloc(array_size * sizeof(mpz_t *));
    
    gmp_randstate_t state;
    gmp_randinit_default(state);

    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array_1[i]);
        mpz_urandomb(mpz_array_1[i], state, 256);
    }
}

// Run after benchmark
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

void bench_mpz_clear_prep(void *argptr)
{
    mpz_size_ = ((int *)argptr)[0];
    int64_t array_size = ((int *)argptr)[1];

    mpz_array = (mpz_t *)malloc(array_size * sizeof(mpz_t *));
    for (uint64_t i = 0; i < array_size; i++)
    {
        mpz_init(mpz_array[i]);
    }
}

void bench_mpz_clear_cleanup(void *argptr)
{
    free(mpz_array);
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_init is analagous to big_int_alloc */
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

/* mpz_clear is analagous to big_int_destroy */
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

/* Copy? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* Prune? */


//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_set_si is similar to the big_int_create function. */
void bench_mpz_set_si_fn(void *arg)
{

}

void bench_mpz_set_si(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_set_str is similar to the big_int_create_from_hex function. */
void bench_mpz_set_str_fn(void *arg)
{

}

void bench_mpz_set_str(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_urandomm is similar to the big_int_create_random function. */
void bench_mpz_urandomm_fn(void *arg)
{

}

void bench_mpz_urandomm(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_set is similar to the big_int_duplicate function. */
void bench_mpz_set_fn(void *arg)
{

}

void bench_mpz_set(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_neg is analagous to the big_int_neg function. */
void bench_mpz_neg_fn(void *arg)
{

}

void bench_mpz_neg(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_abs is analagous to the big_int_abs function. */
void bench_mpz_abs_fn(void *arg)
{

}

void bench_mpz_abs(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_add is analagous to the big_int_add function */
void bench_mpz_add_fn(void *arg)
{

}

void bench_mpz_add(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


/* add mod? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_sub is analagous to the big_int_sub function. */
void bench_mpz_sub_fn(void *arg)
{

}

void bench_mpz_sub(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* sub mod? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_mul is analagous to the big_int_mul function. */
void bench_mpz_mul_fn(void *arg)
{

}

void bench_mpz_mul(void *bench_args, char *bench_name, char *path)
{
    
}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mul mod? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_tdiv_qr is similar to the big_int_div_rem function. */
void bench_mpz_tdiv_qr_fn(void *arg)
{

}

void bench_GMP_mpz_tdiv_qr(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_tdiv_q is similar to the big_int_div funciton. */
void bench_mpz_tdiv_q_fn(void *arg)
{

}

void bench_mpz_tdiv_q(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* div mod? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* left shift? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* right shift? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_mod is analagous to the big_int_mod function. */
void bench_mpz_mod_fn(void *arg)
{

}

void bench_mpz_mod(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_invert is analagous to the big_int_inv function. */
void bench_mpz_invert_fn(void *arg)
{

}

void bench_mpz_invert(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_powm is analagous to the big_int_pow function. */
void bench_mpz_powm_fn(void *arg)
{

}

void bench_mpz_powm(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* Is zero? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* Is odd? */

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_cmp is analagous to the big_int_compare function. */
void bench_mpz_cmp_fn(void *arg)
{

}

void bench_mpz_cmp(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_gcd_ext is analagous to the big_int_egcd function. */
void bench_mpz_gcd_ext_fn(void *arg)
{

}

void bench_mpz_gcd_ext(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

int main(int argc, char const *argv[])
{
    int bench_type;
    for (int i = 1; i < argc; ++i) {
        bench_type = atoi(argv[i]);
        
        BENCHMARK(bench_type, BENCH_TYPE_ALLOC,
            bench_mpz_init((void *)bench_mpz_size_256_args, "init",
                NULL));
        
        BENCHMARK(bench_type, BENCH_TYPE_DESTROY,
            bench_mpz_clear((void *)bench_mpz_size_256_args, "clear",
                NULL));
    }

    return EXIT_SUCCESS;
}
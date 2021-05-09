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

// Include header files
#include "benchmark_helpers.h"

#ifndef LOG_PATH
    #define LOG_PATH "../logs"
#endif

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

void bench_GMP_prep(void *argptr)
{

}

// Run after benchmark
void bench_GMP_cleanup(void *argptr)
{

}

void bench_GMP_small_prep(void *argptr)
{

}

// Run after benchmark
void bench_GMP_small_cleanup(void *argptr)
{

}

void bench_GMP_destroy_prep(void *argptr)
{

}

void bench_GMP_destroy_cleanup(void *argptr)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_init is analagous to big_int_alloc */
void bench_mpz_init_fn(void *arg)
{

}

void bench_mpz_init(void *bench_args, char *bench_name, char *path)
{

}

//--- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

/* mpz_clear is analagous to big_int_destroy */
void bench_mpz_clear_fn(void *arg)
{

}

void bench_mpz_clear(void *bench_args, char *bench_name, char *path)
{

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

int main(void)
{
    printf_bench_header("GMP Benchmarks");
    return EXIT_SUCCESS;
}
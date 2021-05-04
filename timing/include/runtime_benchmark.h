#ifndef RUNTIME_BENCHMARK_H_ /* Include guard */
#define RUNTIME_BENCHMARK_H_

#ifndef LOG_PATH
#define LOG_PATH "../logs"
#endif

#ifndef BIGINTSIZE
#define BIGINTSIZE 8
#endif

#ifndef SETS
#define SETS 10
#endif

#ifndef REPS
#define REPS 10
#endif

#ifndef MICROREPS
#define MICROREPS 1000
#endif

#ifndef NONZEROBITS
#define NONZEROBITS 32
#endif

#include "bigint.h"
#include "elligator.h"

/*
 * Global BigInt variables
 */
BigInt **big_int_array;
BigInt **big_int_array_1;
BigInt **big_int_array_2;
BigInt **big_int_array_3;
BigInt **big_int_array_4;
BigInt **big_int_array_q;

Curve bench_curve;
CurvePoint *curve_point_array;

int8_t *int8_t_array_1;
uint64_t *uint64_t_array_1;

int big_int_size_;
int big_int_array_size_;

int bench_big_int_size_256_args[] = {BIGINTSIZE, MICROREPS, 1};
int bench_big_int_size_256_random_mod_args[] = {BIGINTSIZE, MICROREPS, 1};
int bench_big_int_size_256_curve_mod_args[] = {BIGINTSIZE, MICROREPS, 0};

void bench_big_int_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    int random_q = ((int *)argptr)[2];

    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_2 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_3 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_4 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_q = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    int8_t_array_1 = (int8_t *)malloc(big_int_array_size_ * sizeof(int8_t));
    uint64_t_array_1 = (uint64_t *)malloc(big_int_array_size_ * sizeof(uint64_t));

    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {

        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        big_int_array_2[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        big_int_array_3[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        big_int_array_4[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);

        if (random_q)
        {
            big_int_array_q[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
        }
        else
        {
            big_int_array_q[i] = big_int_create_from_hex(NULL, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
            ;
        }

        uint64_t_array_1[i] = rand() % 256;
    }
}

// Run after benchmark
void bench_big_int_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array_1[i]);
        big_int_destroy(big_int_array_2[i]);
        big_int_destroy(big_int_array_3[i]);
        big_int_destroy(big_int_array_4[i]);
        big_int_destroy(big_int_array_q[i]);
    }
    free(big_int_array);
    free(big_int_array_1);
    free(big_int_array_2);
    free(big_int_array_3);
    free(big_int_array_4);
    free(big_int_array_q);
    
    free(int8_t_array_1);
    free(uint64_t_array_1);
}


void bench_big_int_small_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    big_int_array_1 = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
    }
}

// Run after benchmark
void bench_big_int_small_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
        big_int_destroy(big_int_array_1[i]);
    }
    free(big_int_array);
    free(big_int_array_1);
}


void bench_big_int_destroy_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array[i] = big_int_alloc(big_int_size_);
    }
}

void bench_big_int_destroy_cleanup(void)
{
    free(big_int_array);
}


void bench_elligator_1_string_to_point_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));

    init_curve1174(&bench_curve);
    curve_point_array = (CurvePoint *)malloc(big_int_array_size_ * sizeof(CurvePoint));


    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
    }
}

// Run after benchmark
void bench_elligator_1_string_to_point_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
        big_int_destroy(big_int_array_1[i]);
    }
    free(big_int_array);
}


void bench_elligator_1_point_to_string_prep(void *argptr)
{
    big_int_size_ = ((int *)argptr)[0];
    big_int_array_size_ = ((int *)argptr)[1];
    big_int_array = (BigInt **)malloc(big_int_array_size_ * sizeof(BigInt *));
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_array_1[i] = big_int_create_random(NULL, big_int_size_, NONZEROBITS);
    }
}

// Run after benchmark
void bench_elligator_1_point_to_string_cleanup(void)
{
    for (uint64_t i = 0; i < big_int_array_size_; i++)
    {
        big_int_destroy(big_int_array[i]);
        big_int_destroy(big_int_array_1[i]);
    }
    free(big_int_array);
}




#endif // RUNTIME_BENCHMARK_H_
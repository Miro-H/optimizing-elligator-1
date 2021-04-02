#ifndef ELLIGATOR_H_    /* Include guard */
#define ELLIGATOR_H_
#include <stdint.h> 

typedef struct BigInts // This needs to be changed to allow the use of 256 bit integers
{
    uint64_t x;
} BigInt;

typedef struct CurvePoints
{
    BigInt x;
    BigInt y;
} CurvePoint;

typedef struct Curves // edward curve
{
    BigInt q; // prime field
    BigInt d; // d of curve
    BigInt s; // s of curve

    BigInt c; // c = 2/(s^2)
    BigInt r; // r = c + 1/c
} Curve;

typedef struct egcd_results {
    BigInt g;
    BigInt y;
    BigInt x;
} egcd_result;


BigInt create_big_int(uint64_t x); // create BigInt from unsigned
BigInt create_big_int_string(char* s); // create BigInt from string

BigInt big_int_pow(BigInt base, BigInt exponent, BigInt q); // base^exponent mod q

//BigInt big_int_mul(BigInt a, BigInt b, BigInt q); // a * b mod q
BigInt big_int_mul(BigInt a, BigInt b); // a * b might be removed

//BigInt big_int_div(BigInt a, BigInt b, BigInt q); // a // b mod q (integer division)
BigInt big_int_div(BigInt a, BigInt b); // a // b might be removed (integer division)

//BigInt big_int_add(BigInt a, BigInt b, BigInt q); // a + b mod q
BigInt big_int_add(BigInt a, BigInt b); // a + b might be removed

//BigInt big_int_sub(BigInt a, BigInt b, BigInt q); // a - b mod q
BigInt big_int_sub(BigInt a, BigInt b); // a - b might be removed

//BigInt big_int_negate(BigInt a, BigInt q); // -a mod q
BigInt big_int_negate(BigInt a); // -a might be removed

BigInt big_int_inverse(BigInt a, BigInt q); // a^(-1) mod q mod inverse

BigInt big_int_mod(BigInt a, BigInt q); // a mod q

uint64_t big_int_compare(BigInt a, BigInt b); // a==b: 0, a<b: -1, a>b: 1

egcd_result egcd(BigInt a, BigInt b); // Euclidean algorithm for gcd computation

BigInt chi(BigInt t, BigInt q); // chi function: chi(t) = t**((q-1)/2)

CurvePoint elligator_1_string_to_point(BigInt t, Curve curve);

BigInt elligator_1_point_to_string(CurvePoint p, Curve curve);


#endif // ELLIGATOR_H_
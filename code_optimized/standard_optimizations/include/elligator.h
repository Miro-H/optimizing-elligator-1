#ifndef ELLIGATOR_H_    /* Include guard */
#define ELLIGATOR_H_

#include <stdint.h>
#include "bigint.h"

typedef struct CurvePoint
{
    BigInt x;
    BigInt y;
} CurvePoint;

typedef struct Curve // edward curve
{
    BigInt q; // prime field
    BigInt d; // d of curve
    BigInt s; // s of curve

    BigInt c; // c = 2/(s^2)
    BigInt c_minus_1; // c - 1
    BigInt c_minus_1_s; // (c - 1) * s
    BigInt c_squared_inverse; // 1/c^2
    BigInt r; // r = c + 1/c
    BigInt r_squared; // r**2
    BigInt r_squared_minus_two; // r**2 - 2
} Curve;

// Special BigInts
__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_two, 0, 0, 1, (2));

__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_three, 0, 0, 1, (3));

__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_four, 0, 0, 1, (4));

__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_five, 0, 0, 1, (5));

void init_curve1174(Curve *curve);

CurvePoint *elligator_1_string_to_point(CurvePoint *curve_point, BigInt *t, Curve curve);
BigInt *elligator_1_point_to_string(BigInt *t, CurvePoint p, Curve curve);

#endif // ELLIGATOR_H_

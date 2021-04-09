#ifndef ELLIGATOR_H_    /* Include guard */
#define ELLIGATOR_H_
#include <stdint.h>

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

CurvePoint elligator_1_string_to_point(BigInt t, Curve curve);

BigInt elligator_1_point_to_string(CurvePoint p, Curve curve);


#endif // ELLIGATOR_H_

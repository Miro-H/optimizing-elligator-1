#ifndef ELLIGATOR_H_    /* Include guard */
#define ELLIGATOR_H_

#include <stdint.h>
#include "bigint.h"

typedef struct CurvePoints
{
    BigInt *x;
    BigInt *y;
} CurvePoint;

typedef struct Curves // edward curve
{
    BigInt *q; // prime field
    BigInt *d; // d of curve
    BigInt *s; // s of curve

    BigInt *c; // c = 2/(s^2)
    BigInt *r; // r = c + 1/c
} Curve;


static dbl_chunk_size_t chunk_two = 2;
static dbl_chunk_size_t chunk_three = 3;
static dbl_chunk_size_t chunk_four = 4;
static dbl_chunk_size_t chunk_five = 5;

// Special BigInts, never free those! They cannot be copied.
__attribute__((unused)) static BigInt *big_int_two = &((BigInt) {
    .sign = 0,
    .overflow = 0,
    .size = 1,
    .alloc_size = 0,
    .chunks = &chunk_two,
});
__attribute__((unused)) static BigInt *big_int_three = &((BigInt) {
    .sign = 0,
    .overflow = 0,
    .size = 1,
    .alloc_size = 0,
    .chunks = &chunk_three,
});
__attribute__((unused)) static BigInt *big_int_four = &((BigInt) {
    .sign = 0,
    .overflow = 0,
    .size = 1,
    .alloc_size = 0,
    .chunks = &chunk_four,
});
__attribute__((unused)) static BigInt *big_int_five = &((BigInt) {
    .sign = 0,
    .overflow = 0,
    .size = 1,
    .alloc_size = 0,
    .chunks = &chunk_five,
});


CurvePoint elligator_1_string_to_point(BigInt *t, Curve curve);

BigInt* elligator_1_point_to_string(CurvePoint p, Curve curve);


#endif // ELLIGATOR_H_

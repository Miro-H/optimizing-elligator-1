#ifndef GMP_ELLIGATOR_H_    /* Include guard */
#define GMP_ELLIGATOR_H_

#include <stdint.h>
#include <gmp.h>

typedef struct GMP_CurvePoint
{
    mpz_t x;
    mpz_t y;
} GMP_CurvePoint;

typedef struct GMP_Curve // edward curve
{
    mpz_t q; // prime field
    mpz_t d; // d of curve
    mpz_t s; // s of curve

    mpz_t c; // c = 2/(s^2)
    mpz_t r; // r = c + 1/c
} GMP_Curve;

void gmp_init_curve1174(GMP_Curve *curve);
void gmp_free_curve(GMP_Curve *curve);
void gmp_free_curve_point(GMP_CurvePoint *point);

GMP_CurvePoint *gmp_elligator_1_string_to_point(GMP_CurvePoint *curve_point, mpz_t t, GMP_Curve curve);
mpz_t *gmp_elligator_1_point_to_string(mpz_t t, GMP_CurvePoint p, GMP_Curve curve);

#endif // GMP_ELLIGATOR_H_

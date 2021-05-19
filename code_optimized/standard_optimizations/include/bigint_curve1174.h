#ifndef BIGINT_CURVE1174_H_    /* Include guard */
#define BIGINT_CURVE1174_H_

#include "bigint_types.h"
#include "bigint.h"

#define Q_CHUNKS 8
#define Q_MSB_CHUNK 0x07ffffff

/**
 * Hard coded prime q = 2^251 - 9 of Curve1174
 */
static BigInt q_bigint = (BigInt) {
    .sign       = 0,
    .overflow   = 0,
    .size       = Q_CHUNKS,
    .pad        = 0,
    .chunks     = { 0xfffffff7, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                    0xffffffff, 0xffffffff, Q_MSB_CHUNK }
};

__attribute__((unused))
static BigInt *q = &q_bigint;

__attribute__((unused))
BIG_INT_DEFINE_STATIC_STRUCT_PTR(big_int_288, 0, 0, 1, 288);

// Modular arithmetic. Note q = 2^251 - 9 is FIXED for all the functions below!
BigInt *big_int_curve1174_mod(BigInt *r, BigInt *a);
BigInt *big_int_curve1174_add_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_sub_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_mul_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_div_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_inv(BigInt *r, BigInt *a);

// Special comparison
int8_t big_int_curve1174_compare_to_q(BigInt *a);

// Advanced operations
BigInt *big_int_curve1174_pow(BigInt *r, BigInt *b, BigInt *e);
EgcdResult *big_int_curve1174_egcd(EgcdResult *r, BigInt *a);
BigInt *big_int_curve1174_chi(BigInt *r, BigInt *t);

#endif // BIGINT_CURVE1174_H_

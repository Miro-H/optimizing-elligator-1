#ifndef BIGINT_CURVE1174_H_    /* Include guard */
#define BIGINT_CURVE1174_H_

#include "bigint_types.h"
#include "bigint.h"
#include "bigint_curve1174_constants.h"

// This is set to the faster algorithm for computing inverses
// #define BIG_INT_CURVE1174_INV(r, a) big_int_curve1174_inv_fermat((r), (a))
#define BIG_INT_CURVE1174_INV(r, a) big_int_inv((r), (a), q)

// Modular arithmetic. Note q = 2^251 - 9 is FIXED for all the functions below!
BigInt *big_int_curve1174_mod(BigInt *r);
BigInt *big_int_curve1174_add_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_sub_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_mul_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_div_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_inv_fermat(BigInt *r, BigInt *a);

// Special comparison
int8_t big_int_curve1174_compare_to_q(BigInt *a);
int8_t big_int_curve1174_lt_aq(BigInt *b, BigInt *aq);

// Advanced operations
BigInt *big_int_curve1174_pow_small(BigInt *r, BigInt *b, uint64_t e);
BigInt *big_int_curve1174_pow(BigInt *r, BigInt *b, BigInt *e);
BigInt *big_int_curve1174_pow_q_m1_d2(BigInt *r, BigInt *b);
BigInt *big_int_curve1174_pow_q_p1_d4(BigInt *r, BigInt *b);
int8_t big_int_curve1174_chi(BigInt *t);

#endif // BIGINT_CURVE1174_H_

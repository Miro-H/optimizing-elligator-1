#ifndef BIGINT_CURVE1174_H_    /* Include guard */
#define BIGINT_CURVE1174_H_

#include "bigint_types.h"
#include "bigint.h"

// Modular arithmetic. Note q = 2^251 - 9 is FIXED for all the functions below!
BigInt *big_int_curve1174_mod(BigInt *r, BigInt *a);
BigInt *big_int_curve1174_add_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_sub_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_mul_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_div_mod(BigInt *r, BigInt *a, BigInt *b);
BigInt *big_int_curve1174_inv(BigInt *r, BigInt *a);

// Advanced operations
BigInt *big_int_curve1174_pow(BigInt *r, BigInt *b, BigInt *e);
EgcdResult *big_int_curve1174_egcd(EgcdResult *r, BigInt *a);
BigInt *big_int_curve1174_chi(BigInt *r, BigInt *t);

#endif // BIGINT_CURVE1174_H_

/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This file implements the BigInt library to support arithmetic operations on
 * 256-bit integers.
 */

/*
 * Includes
 */
#include <stdio.h>
#include <assert.h>

// Include header files
#include "bigint.h"


/**
 * \brief Create BigInt from string
 */
BigInt create_big_int_string(char* s)
{
    assert("not yet implemented!");
    return create_big_int(-1);
}

// TODO: change for 256 bits
BigInt create_big_int(uint64_t x)
{
    BigInt r = {x};
    return r;
}

/**
 * \brief Calculate base^exponent mod q for BigInts
 */
BigInt big_int_pow(BigInt base, BigInt exponent, BigInt q)
{
    BigInt result = create_big_int(1);
    BigInt p = exponent;
    BigInt b = base;

    while (big_int_compare(p, create_big_int(0)) > 0)
    {
        // If power is odd
        if (big_int_compare(big_int_mod(p, create_big_int(2)), create_big_int(1)) == 0)
        {
            result = big_int_mod(big_int_mul(result, b), q);
        }
        p = big_int_div(p, create_big_int(2));
        p = big_int_mod(big_int_mul(b, b), q);
    }
    return result;
}

/**
 * \brief Calculate a * b
 */
 // TODO: add mul_mod function, might remove this one
BigInt big_int_mul(BigInt a, BigInt b)
{
    BigInt r = {a.x * b.x};
    return r;
}

/**
 * \brief Calculate a // b (integer division)
 */
BigInt big_int_div(BigInt a, BigInt b)
{
    BigInt r = {a.x / b.x};
    return r;
}

/**
 * \brief Calculate (a // b) mod q (integer division mod q)
 */
// TODO: uncomment when big_int_mul_mod exists
// BigInt big_int_div(BigInt a, BigInt b, BigInt q)
// {
//     return big_int_mul_mod(a, big_int_inverse(b, q), q);
// }


/*
BigInt big_int_add(BigInt a, BigInt b, BigInt q) // needs to be changed
{
    return BigInt((a.x + b.x) % q.x);
}
*/
BigInt big_int_add(BigInt a, BigInt b) // needs to be changed
{
    BigInt r = {a.x + b.x};
    return r;
}
/*
BigInt big_int_sub(BigInt a, BigInt b, BigInt q) // needs to be changed
{
    return BigInt((a.x - b.x) % q.x);
}
*/
BigInt big_int_sub(BigInt a, BigInt b) // needs to be changed
{
    BigInt r = {a.x - b.x};
    return r;
}
/*
BigInt big_int_negate(BigInt a, BigInt q) // needs to be changed
{
    return BigInt((-a.x) % q.x);
}
*/
BigInt big_int_negate(BigInt a) // needs to be changed
{
    BigInt r = {-a.x};
    return r;
}

BigInt big_int_inverse(BigInt a, BigInt q) // a^(-1) mod q mod inverse
{
    // Modular inversion computation
    if (big_int_compare(a, create_big_int(0)) < 0)
    {
        BigInt r0 = big_int_negate(a);
        r0 = big_int_inverse(r0, q);
        return big_int_sub(q, r0);
    }

    egcd_result res = egcd(a, q);
    if (big_int_compare(q, create_big_int(1)) != 0)
    {
        return create_big_int(-1);
    }
    else
    {
        return big_int_mod(res.x, q);
    }
}

BigInt big_int_mod(BigInt a, BigInt q) // needs to be changed
{
    BigInt r = {a.x % q.x};
    return r;
}

uint64_t big_int_compare(BigInt a, BigInt b) // a==b: 0, a<b: -1, a>b: 1
{
    if (a.x == b.x)
    {
        return 0;
    }
    else if (a.x < b.x)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

egcd_result egcd(BigInt a, BigInt b) // Euclidean algorithm for gcd computation
{
    if (big_int_compare(a, create_big_int(0)) == 0)
    {
        egcd_result r = {b, create_big_int(0), create_big_int(1)};
        return r;
    }
    else
    {
        egcd_result res = egcd(big_int_mod(b, a), a);
        BigInt r0 = big_int_div(b, a);
        r0 = big_int_mul(r0, res.y);
        egcd_result r = {res.g, big_int_sub(res.x, r0), res.y};
        return r;
    }
}

BigInt chi(BigInt t, BigInt q) // chi function: chi(t) = t**((q-1)/2)
{
    BigInt r0 = big_int_inverse(create_big_int(2), q);
    BigInt r1 = big_int_sub(q, create_big_int(1));
    r0 = big_int_mul(r1, r0);
    r0 = big_int_pow(t, r0, q);
    return r0;
}

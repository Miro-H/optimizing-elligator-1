/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This is a unit test file to test the BigInt implementation.
 */

/*
 * Includes
 */
#include <check.h>
#include <stdlib.h>

// Include header files
#include "bigint.h"
#include "debug.h"

// Create macro to define a BigInt chunk according to the different APIs of
// different versions of our code
#if VERSION == 1

#define TEST_BIG_INT_DEFINE(v) BigInt *v = big_int_create_from_chunk(NULL, 0, 0)
#define TEST_BIG_INT_DESTROY(v) big_int_destroy(v)
#define TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp)                     \
    do {                                                                       \
        ck_assert_int_eq(big_int_compare((res).g, (g_exp)), 0);                \
        ck_assert_int_eq(big_int_compare((res).x, (x_exp)), 0);                \
        ck_assert_int_eq(big_int_compare((res).y, (y_exp)), 0);                \
                                                                               \
        big_int_destroy((res).g);                                              \
        big_int_destroy((res).x);                                              \
        big_int_destroy((res).y);                                              \
    } while (0)

#else

#define TEST_BIG_INT_DEFINE(v) BIG_INT_DEFINE_FROM_CHUNK(v, 0, 0)
#define TEST_BIG_INT_DESTROY(v) ((void) 0)
#define TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp)                     \
    do {                                                                       \
        ck_assert_int_eq(big_int_compare(&(res).g, (g_exp)), 0);               \
        ck_assert_int_eq(big_int_compare(&(res).x, (x_exp)), 0);               \
        ck_assert_int_eq(big_int_compare(&(res).y, (y_exp)), 0);               \
    } while (0)

#endif

/**
* \brief Test creating a BigInt from an int64_t
*/
START_TEST(test_create_from_int64)
{
    TEST_BIG_INT_DEFINE(a);

    chunk_size_t t;

    // Test positive integer
    t = 12390481;
    big_int_create_from_chunk(a, t, 0);
    ck_assert_uint_eq(a->chunks[0], t);
    ck_assert_uint_eq(a->sign, 0);

    // Test negative integer
    t = 3412123;
    big_int_create_from_chunk(a, t, 1);
    ck_assert_uint_eq(a->chunks[0], t);
    ck_assert_uint_eq(a->sign, 1);

    TEST_BIG_INT_DESTROY(a);
}
END_TEST


/**
* \brief Test creating a BigInt from a hex string
*/
START_TEST(test_create_from_hex)
{
    TEST_BIG_INT_DEFINE(a);

    // Test positive integer
    big_int_create_from_hex(a, "DEADBEEF");
    ck_assert_uint_eq(a->chunks[0], 0xDEADBEEF);
    ck_assert_uint_eq(a->sign, 0);

    // Test negative integer
    big_int_create_from_hex(a, "-C0FFEE");
    ck_assert_uint_eq(a->chunks[0], 0xC0FFEE);
    ck_assert_uint_eq(a->sign, 1);

    // Test negative integer of chunk size
    big_int_create_from_hex(a, "-12345678");
    ck_assert_uint_eq(a->chunks[0], 0x12345678);
    ck_assert_uint_eq(a->sign, 1);

    // Test multi-chunk positive integer
    big_int_create_from_hex(a, "F050000000000000000F00D");
    ck_assert_uint_eq(a->chunks[0], 0x0000F00D);
    ck_assert_uint_eq(a->chunks[1], 0x00000000);
    ck_assert_uint_eq(a->chunks[2], 0x0F050000);
    ck_assert_uint_eq(a->size, 3);
    ck_assert_uint_eq(a->sign, 0);

    // Test multi-chunk negative integer
    big_int_create_from_hex(a, "-F050000000000000000F00D");
    ck_assert_uint_eq(a->chunks[0], 0x0000F00D);
    ck_assert_uint_eq(a->chunks[1], 0x00000000);
    ck_assert_uint_eq(a->chunks[2], 0x0F050000);
    ck_assert_uint_eq(a->size, 3);
    ck_assert_uint_eq(a->sign, 1);

    TEST_BIG_INT_DESTROY(a);
}
END_TEST


/**
* \brief Test negating BigInt by flipping the sign twice, and checking that we
* arrive at the same number again
*/
START_TEST(test_negate)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(a_neg);

    big_int_create_from_chunk(a, 123, 0);

    // Flip 1
    big_int_neg(a_neg, a);
    ck_assert_uint_eq(a->chunks[0], a_neg->chunks[0]);
    ck_assert_uint_ne(a->sign, a_neg->sign);

    // Flip 2
    big_int_neg(a_neg, a_neg);
    ck_assert_int_eq(big_int_compare(a, a_neg), 0);
    ck_assert_uint_eq(a->sign, a_neg->sign);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(a_neg);
}
END_TEST


/**
* \brief Test addition of BigInts
*/
START_TEST(test_addition)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(r);

    // normal addition of two positive integers
    big_int_create_from_chunk(a, 123, 0);
    big_int_create_from_chunk(b, 123412, 0);
    big_int_create_from_chunk(r, 123535, 0);

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // addition with single chunk overflow
    big_int_create_from_chunk(a, 4294967290, 0);
    big_int_create_from_chunk(b, 16, 0);
    big_int_create_from_hex(r, "10000000A");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // normal addition of a positive and a negative integers
    big_int_create_from_chunk(a, 123, 0);
    big_int_create_from_chunk(b, 123412, 1);
    big_int_create_from_chunk(r, 123289, 1);

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // normal addition of a negative and a positive integers
    big_int_create_from_chunk(a, 89545823, 1);
    big_int_create_from_chunk(b, 89545823, 0);

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, big_int_zero);
    ck_assert_int_eq(big_int_compare(a, big_int_zero), 0);

    // normal addition of a negative and a positive integers
    big_int_create_from_chunk(a, 4, 1);
    big_int_create_from_chunk(b, 2, 1);
    big_int_create_from_chunk(r, 6, 1);

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, r);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // addition with single chunk underflow
    big_int_create_from_chunk(a, 4294967295, 1);
    big_int_create_from_chunk(b, 2, 1);
    big_int_create_from_hex(r, "-100000001");

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, big_int_zero);
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // multi-chunk addition of two positive integers
    big_int_create_from_hex(a, "A20B9BDB69E6C331825D79743C398D0E");
    big_int_create_from_hex(b, "293D794457EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "CB49151FC1D15EFB98415BFAEFD30E84");

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, r);
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // multi-chunk addition of one positive integer and one negative integer
    big_int_create_from_hex(a, "A20B9BDB69E6C331825D79743C398D0E");
    big_int_create_from_hex(b, "-293D794457EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "78CE229711FC27676C7996ED88A00B98");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // multi-chunk addition of one negative integer and one positive integer
    big_int_create_from_hex(a, "-A20B9BDB69E6C331825D79743C398D0E");
    big_int_create_from_hex(b, "293D794457EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "-78CE229711FC27676C7996ED88A00B98");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // multi-chunk addition of two negative integers
    big_int_create_from_hex(a, "-A20B9BDB69E6C331825D79743C398D0E");
    big_int_create_from_hex(b, "-293D794457EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "-CB49151FC1D15EFB98415BFAEFD30E84");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // Test mixed sized BigInt ops
    big_int_create_from_hex(a, "A20B9BDB69E6C331743C398D0E");
    big_int_create_from_hex(b, "29315E3E286B3998176");
    big_int_create_from_hex(r, "A20B9BDDFCFCA713FAEFD30E84");

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, r);
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    big_int_create_from_hex(a, "0E");
    big_int_create_from_hex(b, "-794457EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "-794457EA9BCA15E3E286B3998168");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    big_int_create_from_hex(a, "-43C398D0E");
    big_int_create_from_hex(b, "57EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "57EA9BCA15E3E282775FF468");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    big_int_create_from_hex(a, "-A20B9BDB69E6C331825D79743C398D0E");
    big_int_create_from_hex(b, "-A15E3E286B3998176");
    big_int_create_from_hex(r, "-A20B9BDB69E6C33B98415BFAEFD30E84");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test subtractions of BigInts
*/
START_TEST(test_subtraction)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(r);

    // normal subtraction of two positive chunks
    big_int_create_from_chunk(a, 123, 0);
    big_int_create_from_chunk(b, 123412, 0);
    big_int_create_from_chunk(r, 123289, 1);

    big_int_sub(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // normal subtraction of two negative chunks
    big_int_create_from_chunk(a, 123, 1);
    big_int_create_from_chunk(b, 123412, 1);
    big_int_create_from_chunk(r, 123289, 0);

    big_int_sub(a, a, b); // a = a - b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // multi-chunk subtraction of two positive numbers
    big_int_create_from_hex(a, "BA2980E4A996ED0AAEA5B0E3B65A7048");
    big_int_create_from_hex(b, "531FEC5ED503B8D3");
    big_int_create_from_hex(r, "BA2980E4A996ED0A5B85C484E156B775");

    big_int_sub(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // multi-chunk subtraction of two negative numbers
    big_int_create_from_hex(a, "-BA2980E4A996ED0AAEA5B0E3B65A7048");
    big_int_create_from_hex(b, "-531FEC5ED503B8D3");
    big_int_create_from_hex(r, "-ba2980e4a996ed0a5b85c484e156b775");

    big_int_sub(a, a, b); // a = a - b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // mixed signs (uses addition underneath, but test pos/neg and neg/pos)
    big_int_create_from_hex(a, "BA2980E4A996ED0AAEA5B0E3B65A7048");
    big_int_create_from_hex(b, "-531FEC5ED503B8D3");
    big_int_create_from_hex(r, "ba2980e4a996ed0b01c59d428b5e291b");

    big_int_sub(a, a, b); // a = a - b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    big_int_create_from_hex(a, "-BA2980E4A996ED0AAEA5B0E3B65A7048");
    big_int_create_from_hex(b, "531FEC5ED503B8D3");
    big_int_create_from_hex(r, "-ba2980e4a996ed0b01c59d428b5e291b");

    big_int_sub(a, a, b); // a = a - b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test comparison of BigInts
*/
START_TEST(test_comparison)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);

    // +0 and -0 comparisons
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_chunk(b, 0, 1);
    ck_assert_int_eq(big_int_compare(a, b), 0);

    big_int_create_from_chunk(a, 0, 1);
    big_int_create_from_chunk(b, 0, 0);
    ck_assert_int_eq(big_int_compare(a, b), 0);

    // a < b
    a = big_int_create_from_chunk(a, 123, 0);
    b = big_int_create_from_chunk(b, 123456, 0);
    ck_assert_int_lt(big_int_compare(a, b), 0);

    // a > b
    a = big_int_create_from_chunk(a, 123456789, 0);
    b = big_int_create_from_chunk(b, 1234, 0);
    ck_assert_int_gt(big_int_compare(a, b), 0);

    // same size
    a = big_int_create_from_hex(a, "A20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "BA2980E4A996ED0AAEA5B0E3B65A7048");
    ck_assert_int_lt(big_int_compare(a, b), 0);

    a = big_int_create_from_hex(a, "F20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "BA2980E4A996ED0AAEA5B0E3B65A7048");
    ck_assert_int_gt(big_int_compare(a, b), 0);

    // different sizes
    a = big_int_create_from_hex(a, "B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "BA2980E4A996ED0AAEA5B0E3B65A7048");
    ck_assert_int_lt(big_int_compare(a, b), 0);

    a = big_int_create_from_hex(a, "A20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "D0AAEA5B0E3B65A7048");
    ck_assert_int_gt(big_int_compare(a, b), 0);

    // mixed signs
    a = big_int_create_from_hex(a, "-A20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "D0AAEA5B0E3B65A7048");
    ck_assert_int_lt(big_int_compare(a, b), 0);

    a = big_int_create_from_hex(a, "A20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "-BA2980E4A996ED0AAEA5B0E3B65A7048");
    ck_assert_int_gt(big_int_compare(a, b), 0);

    // both negative
    a = big_int_create_from_hex(a, "-F20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "-BA2980E4A996ED0AAEA5B0E3B65A7048");
    ck_assert_int_lt(big_int_compare(a, b), 0);

    // very small difference
    a = big_int_create_from_hex(a, "F20B9BDB69E6C33B98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "F20B9BDB69E6C33B98415BFAEFD30E85");
    ck_assert_int_lt(big_int_compare(a, b), 0);

    a = big_int_create_from_hex(a, "F20B9BDB69E6C33B98415BFAEFD30E86");
    b = big_int_create_from_hex(b, "F20B9BDB69E6C33B98415BFAEFD30E85");
    ck_assert_int_gt(big_int_compare(a, b), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
}
END_TEST

/**
* \brief Test multiplication of BigInts
*/
START_TEST(test_multiplication)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(c);
    TEST_BIG_INT_DEFINE(r);

    // single chunk multiplication
    big_int_create_from_chunk(a, 31180, 0);
    big_int_create_from_chunk(b, 22299, 0);
    big_int_create_from_chunk(r, 695282820, 0);

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    // multi-chunk multiplication
    big_int_create_from_hex(a, "859CC30B50220F05");
    big_int_create_from_hex(b, "-F623FE531DA1D2B3");
    big_int_create_from_hex(r, "-80776C98745C5CEA986DBE62FB479A7F");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Zero multiplication
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_hex(b, "-F623FE531DA1D2B3");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, big_int_zero), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Multiplications with zero chunks
    big_int_create_from_hex(a, "859CC30B00000000");
    big_int_create_from_hex(b, "F623FE531DA1D2B3");
    big_int_create_from_hex(r, "80776C9827505E37ED8666B100000000");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Multiply integers of different sizes
    a = big_int_create_from_hex(a, "98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "D0AAEA");
    r = big_int_create_from_hex(r, "7C1AC14EB5921D29CEECA8");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Multiply integers with mixed signs
    a = big_int_create_from_hex(a, "-98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "D0AAEA");
    r = big_int_create_from_hex(r, "-7C1AC14EB5921D29CEECA8");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    a = big_int_create_from_hex(a, "98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "-D0AAEA");
    r = big_int_create_from_hex(r, "-7C1AC14EB5921D29CEECA8");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Multiply two negative integers
    a = big_int_create_from_hex(a, "-98415BFAEFD30E84");
    b = big_int_create_from_hex(b, "-D0AAEA");
    r = big_int_create_from_hex(r, "7C1AC14EB5921D29CEECA8");

    big_int_mul(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(c);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

#if VERSION > 1
/**
 * \brief Test multiplication of BigInts where one input has only one chunk.
 */
START_TEST(test_multiplication_single_chunk)
{
    TEST_BIG_INT_DEFINE(a);
    dbl_chunk_size_t b;
    TEST_BIG_INT_DEFINE(r);
    TEST_BIG_INT_DEFINE(c);

    // single chunk multiplication
    big_int_create_from_chunk(a, 31180, 0);
    b = 22299;
    big_int_create_from_chunk(r, 695282820, 0);

    big_int_mul_single_chunk(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    // multi-chunk multiplication
    big_int_create_from_hex(a, "859CC30B50220F05");
    b = 12345;
    big_int_create_from_hex(r, "192b24798c8b3a66481d");

    big_int_mul_single_chunk(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Zero multiplication
    big_int_create_from_chunk(a, 0, 0);
    b = 152637;

    big_int_mul_single_chunk(c, a, b);
    ck_assert_int_eq(big_int_compare(c, big_int_zero), 0);
    ck_assert_uint_eq(c->overflow, 0);

    // Multiplications with zero chunks
    big_int_create_from_hex(a, "859CC30B00000000");
    b = 123;
    big_int_create_from_hex(r, "403251b64900000000");

    // Multiply integers with mixed signs
    a = big_int_create_from_hex(a, "-98415BFAEFD30E84");
    b = 13675242;
    r = big_int_create_from_hex(r, "-7C1AC14EB5921D29CEECA8");

    big_int_mul_single_chunk(c, a, b);
    ck_assert_int_eq(big_int_compare(c, r), 0);
    ck_assert_uint_eq(c->overflow, 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(r);
    TEST_BIG_INT_DESTROY(c);
}
END_TEST

START_TEST(test_squared)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(r);

    // Single chunk
    big_int_create_from_chunk(a, 67, 0);
    big_int_create_from_chunk(r, 4489, 0);

    big_int_square(b, a);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Multi-chunk
    big_int_create_from_hex(a, "ABCDEF123456789ABCDEF");
    big_int_create_from_hex(r, "734CC30B145656A0BC31724B50FCA5E20890F2A521");

    big_int_square(b, a);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Check: Does it give same output as mul?
    big_int_create_from_hex(a, "ABCDEF123456789ABCDEF");
    big_int_mul(r, a, a);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST
#endif

/**
* \brief Test division of BigInts
*/
START_TEST(test_division)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r_exp);
    TEST_BIG_INT_DEFINE(q_exp);
    TEST_BIG_INT_DEFINE(r);

    big_int_create_from_chunk(r, 0, 0);
    big_int_create_from_chunk(r, 0, 0);

    // single chunk division
    big_int_create_from_chunk(a, 4184080774, 0);
    big_int_create_from_chunk(b, 9961738, 0);
    big_int_create_from_chunk(r_exp, 150814, 0);
    big_int_create_from_chunk(q_exp, 420, 0);

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // two chunk division
    big_int_create_from_hex(a, "252314566A9CEBC1");
    big_int_create_from_hex(b, "2487BCEAC2FA6");
    big_int_create_from_hex(q_exp, "1044");
    big_int_create_from_hex(r_exp, "1688C1E5E3A9");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // 4-chunk division
    big_int_create_from_hex(a, "285FB8062273B0CD7F86F076B10720D");
    big_int_create_from_hex(b, "20B3EAD995ED443F46535EA");
    big_int_create_from_hex(q_exp, "13C0CC929");
    big_int_create_from_hex(r_exp, "AFA55DFE42B1F6708E1593");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // Mixed sign division
    big_int_create_from_chunk(a, 33, 1);
    big_int_create_from_chunk(b, 4, 0);
    big_int_create_from_chunk(q_exp, 9, 1);
    big_int_create_from_chunk(r_exp, 3, 0);

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    big_int_create_from_chunk(a, 33, 0);
    big_int_create_from_chunk(b, 4, 1);
    big_int_create_from_chunk(q_exp, 9, 1);
    big_int_create_from_chunk(r_exp, 3, 1);

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    big_int_create_from_hex(a, "-285FB8062273B0CD7F86F076B10720D");
    big_int_create_from_hex(b, "20B3EAD995ED443F46535EA");
    big_int_create_from_hex(q_exp, "-13C0CC92A");
    big_int_create_from_hex(r_exp, "15B994F9B1C224D83D72057");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    big_int_create_from_hex(a, "285FB8062273B0CD7F86F076B10720D");
    big_int_create_from_hex(b, "-20B3EAD995ED443F46535EA");
    big_int_create_from_hex(q_exp, "-13C0CC92A");
    big_int_create_from_hex(r_exp, "-15B994F9B1C224D83D72057");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // Test division of two negative numbers
    big_int_create_from_chunk(a, 33, 1);
    big_int_create_from_chunk(b, 4, 1);
    big_int_create_from_chunk(q_exp, 8, 0);
    big_int_create_from_chunk(r_exp, 1, 1);

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    big_int_create_from_hex(a, "-285FB8062273B0CD7F86F076B10720D");
    big_int_create_from_hex(b, "-20B3EAD995ED443F46535EA");
    big_int_create_from_hex(q_exp, "13c0cc929");
    big_int_create_from_hex(r_exp, "-afa55dfe42b1f6708e1593");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // Divisor larger than dividend
    big_int_create_from_hex(a, "3B0CD7F86F07");
    big_int_create_from_hex(b, "20B3EAD995ED443F46535EA");
    big_int_create_from_chunk(q_exp, 0, 0);
    big_int_create_from_hex(r_exp, "3b0cd7f86f07");

    // Zero dividend
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_hex(b, "20B3EAD995ED443F46535EA");
    big_int_create_from_chunk(q_exp, 0, 0);
    big_int_create_from_chunk(r_exp, 0, 0);

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // TODO: Test 0 in MSB

    // TODO: Test step D6
    // This test passes, but I don't think step D6 is actually being triggered.
    // These are test values for radix 2^16 from the book but I think we are using
    // 2^32, I am not sure how to find my own values :(
    big_int_create_from_hex(a, "7fff800100000000");
    big_int_create_from_hex(b, "800080020005");
    big_int_create_from_hex(q_exp, "fffd");
    big_int_create_from_hex(r_exp, "80008001000f");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    // Not sure what this test case is for.
    big_int_create_from_hex(a, "5F9F737BB0C9873450C3A516A8B5F0F0");
    big_int_create_from_hex(b, "68A753D72F1F907F1BFAF91F0");
    big_int_create_from_hex(q_exp, "E9E8C50");
    big_int_create_from_hex(r_exp, "185E80D262E90C5F8BE915F0");

    big_int_div_rem(q, r, a, b);
    ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
    TEST_BIG_INT_DESTROY(r_exp);
    TEST_BIG_INT_DESTROY(q_exp);
}
END_TEST

/**
* \brief Test left shift of BigInts
*/
START_TEST(test_sll_small)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(r);
    TEST_BIG_INT_DEFINE(t);

    // Shift over chunk border
    big_int_create_from_chunk(a, 4184080774, 0);
    big_int_create_from_hex(r, "3E590061800");

    big_int_sll_small(t, a, 10);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // Shift by 0
    big_int_create_from_chunk(a, 4184080774, 0);
    big_int_create_from_chunk(r, 4184080774, 0);

    big_int_sll_small(t, a, 0);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // Apply shift to 0
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_chunk(r, 0, 0);

    big_int_sll_small(t, a, 10);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // Apply shift to negative integer
    big_int_create_from_chunk(a, 4184080774, 1);
    big_int_create_from_hex(r, "-3E590061800");

    big_int_sll_small(t, a, 10);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(r);
    TEST_BIG_INT_DESTROY(t);
}
END_TEST

/**
* \brief Test right shift of BigInts
*/
START_TEST(test_srl_small)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(r);

    // Shift over chunk border
    big_int_create_from_hex(a, "102CC2711CB04A427");
    big_int_create_from_hex(r, "102CC2711CB04A42");

    big_int_srl_small(a, a, 4);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Shift by 0
    big_int_create_from_chunk(a, 4184080774, 0);
    big_int_create_from_chunk(r, 4184080774, 0);

    big_int_srl_small(a, a, 0);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Apply shift to 0
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_chunk(r, 0, 0);

    big_int_srl_small(a, a, 10);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Apply shift to negative integer
    big_int_create_from_chunk(a, 1000, 1);
    big_int_create_from_chunk(r, 250, 1);

    big_int_srl_small(a, a, 2);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Test whether failure below is due to hex representation
    big_int_create_from_hex(a, "-3e8");
    big_int_create_from_hex(r, "-fa");

    big_int_srl_small(a, a, 2);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Test whether failure occurs with largest possible 1-chunk integer
    big_int_create_from_chunk(a, 4294967295, 1);
    big_int_create_from_chunk(r, 2147483648, 1);

    big_int_srl_small(a, a, 1);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Test shifting negative numbers (round towards -inf)
    big_int_create_from_hex(a, "-80000000");
    big_int_create_from_hex(r, "-1");

    big_int_srl_small(a, a, 32);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a, "-100000001");
    big_int_create_from_hex(r, "-80000001");

    big_int_srl_small(a, a, 1);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a, "-102CC2711CB04A427");
    big_int_create_from_hex(r, "-81661388e5825214");

    big_int_srl_small(a, a, 1);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Shift away all bits
    big_int_create_from_hex(a, "102CC2711CB04A427");
    big_int_create_from_chunk(r, 0, 0);

    big_int_srl_small(a, a, 65);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test modulo operation on BigInts
*/
START_TEST(test_modulo_operation)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r);

    big_int_create_from_hex(a, "D94DD4B70E1C291FAE8BB971F");
    big_int_create_from_hex(q, "1034053B0A");
    big_int_create_from_hex(r, "1A8E7BE75");

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Negative a
    big_int_create_from_hex(a, "-D94DD4B70E1C291FAE8BB971F");
    big_int_create_from_hex(q, "1034053B0A");
    big_int_create_from_hex(r, "e8b1d7c95");

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a = 0
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_hex(q, "1034053B0A");
    big_int_create_from_chunk(r, 0, 0);

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a < q
    big_int_create_from_hex(a, "1034053B0A");
    big_int_create_from_hex(q, "1034053B0A567");
    big_int_create_from_hex(r, "1034053B0A");

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a < q; negative a
    big_int_create_from_hex(a, "-1034053B0A");
    big_int_create_from_hex(q, "1034053B0A567");
    big_int_create_from_hex(r, "103301FAB6A5D");

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // q | a
    big_int_create_from_hex(a, "144229c9ccec0c"); // a = 20 * q
    big_int_create_from_hex(q, "1034EE3B0A567");
    big_int_create_from_chunk(r, 0, 0);

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test addition of BigInts modulo q
*/
START_TEST(test_add_mod)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r);

    // Basic test
    big_int_create_from_hex(a, "ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "7a9e67f62");

    big_int_add_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Mixed signs
    big_int_create_from_hex(a, "-ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "1522f58aa0");

    big_int_add_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a, "ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "-ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "1f2db95bd8");

    big_int_add_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a, "-ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "-ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "2ca6c86716");

    big_int_add_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Test mod operation with intermediate result that is larger than 256 bits
    big_int_create_from_hex(a, "ABCD4569096134AB3096134DAD469B3096134690964DDABCD4569AB3096134DD");
    big_int_create_from_hex(b, "ABCD4569096134AB3096134DAD469B3096134690964DDABCD4569AB3096134DD");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "c71a61b2a");

    big_int_add_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test subtraction of BigInts modulo q
*/
START_TEST(test_sub_mod)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r);

    // Basic test
    big_int_create_from_hex(a, "ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "1f2db95bd8");

    big_int_sub_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Mixed signs
    big_int_create_from_hex(a, "-ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "2ca6c86716");

    big_int_sub_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a, "ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "-ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "7a9e67f62");

    big_int_sub_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a, "-ABCD4569ABEF43096134DD");
    big_int_create_from_hex(b, "-ee543abc7856AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "1522f58aa0");

    big_int_sub_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Test mod operation with intermediate result that is larger than 256 bits
    big_int_create_from_hex(a, "ABCD4569AB3096134DDABCD4569AB3096134DDABCD4569AB3096134DD");
    big_int_create_from_hex(b, "ABCD4569AB3096134DDABCD4569AB3096134DD");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "256A7CCB10");

    big_int_sub_mod(a, a, b, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test multiplication of BigInts modulo q
*/
START_TEST(test_mul_mod)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(c);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r);

    // Basic test
    big_int_create_from_hex(a, "ABCD4569AB3096134DD");
    big_int_create_from_hex(b, "56AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "fec0f30a1");

    big_int_mul_mod(c, a, b, q);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    // Test mod operation with intermediate result that is larger than 256 bits
    a = big_int_create_from_hex(a, "ABCD4569AB3096134DDABCD4569AB3096134DDABCD4569AB3096134DD");
    b = big_int_create_from_hex(b, "ABCD4569AB3096134DDABCD4569AB3096134DD");
    q = big_int_create_from_hex(q, "3450AEE678");
    r = big_int_create_from_hex(r, "1f7046da41");

    big_int_mul_mod(c, a, b, q);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    // Trigger the rare case (D6 in Knuth's Book)
    a = big_int_create_from_hex(a,
            "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    b = big_int_create_from_hex(b,
            "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    q = big_int_create_from_hex(q,
            "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    r = big_int_create_from_hex(r,
            "200000000000000000000000000000000000000000000000000000000000012");

    big_int_mul_mod(c, a, b, q);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    // Mixed signs
    a = big_int_create_from_hex(a, "-ABCD4569ABEF");
    b = big_int_create_from_hex(b, "ee543abc7856AA095");
    q = big_int_create_from_hex(q, "3450AEE678");
    r = big_int_create_from_hex(r, "19d609f6a5");

    big_int_mul_mod(c, a, b, q);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    a = big_int_create_from_hex(a, "ABCD4569ABEF43096134DD");
    b = big_int_create_from_hex(b, "-ee543abc7856AA098765");
    q = big_int_create_from_hex(q, "3450AEE678");
    r = big_int_create_from_hex(r, "321a4c766f");

    big_int_mul_mod(c, a, b, q);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    a = big_int_create_from_hex(a, "-ABCD4569ABEF43096134DD");
    b = big_int_create_from_hex(b, "-ee543abc7856AA098765");
    q = big_int_create_from_hex(q, "3450AEE678");
    r = big_int_create_from_hex(r, "236627009");

    big_int_mul_mod(c, a, b, q);
    ck_assert_int_eq(big_int_compare(c, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(c);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test division of BigInts modulo q
*/
START_TEST(test_div_mod)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r);
    TEST_BIG_INT_DEFINE(x);

    // No pointer reuse
    big_int_create_from_hex(a, "ABCD4569AB3096134DD");
    big_int_create_from_hex(b, "56AA098765");
    big_int_create_from_hex(q, "3450AEE678");
    big_int_create_from_hex(r, "2DD358AAC1");
    big_int_create_from_chunk(x, 0, 0);

    big_int_div_mod(x, a, b, q);
    ck_assert_int_eq(big_int_compare(x, r), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
    TEST_BIG_INT_DESTROY(x);
}
END_TEST

/**
* \brief Test finding the modular inverse of BigInts
*/
START_TEST(test_modulo_inverse)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(ainv_exp);

    big_int_create_from_hex(a, "76101CAD986E75478DAAD160");
    big_int_create_from_hex(q, "C18A71D87958DF7154BABA57");
    big_int_create_from_hex(ainv_exp, "6F3BDAB18DB2D458A39A3BEE");

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    // Negative a
    big_int_create_from_hex(a, "-76101CAD986E75478DAAD160");
    big_int_create_from_hex(q, "C18A71D87958DF7154BABA57");
    big_int_create_from_hex(ainv_exp, "524E9726EBA60B18B1207E69");

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    // Very simple cases
    // a = 7
    big_int_create_from_chunk(a, 7, 0);
    big_int_create_from_chunk(q, 5, 0);
    big_int_create_from_chunk(ainv_exp, 3, 0);

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    // a = 6
    big_int_create_from_chunk(a, 6, 0);
    big_int_create_from_chunk(q, 5, 0);
    big_int_create_from_chunk(ainv_exp, 1, 0);

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    // a = 4
    big_int_create_from_chunk(a, 4, 0);
    big_int_create_from_chunk(q, 5, 0);
    big_int_create_from_chunk(ainv_exp, 4, 0);

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    // a = 3
    big_int_create_from_chunk(a, 3, 0);
    big_int_create_from_chunk(q, 5, 0);
    big_int_create_from_chunk(ainv_exp, 2, 0);

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    // a = 2
    big_int_create_from_chunk(a, 2, 0);
    big_int_create_from_chunk(q, 5, 0);
    big_int_create_from_chunk(ainv_exp, 3, 0);

    big_int_inv(b, a, q);
    ck_assert_int_eq(big_int_compare(b, ainv_exp), 0);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(ainv_exp);
}
END_TEST

/**
* \brief Test raising BigInts to a (BigInt) power
*/
START_TEST(test_power)
{
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(e);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(r);

    big_int_create_from_hex(b, "4F2B8718");
    big_int_create_from_chunk(e, 23, 0);
    big_int_create_from_hex(q, "31DECA5CA5BE11D8DF78F332F");
    big_int_create_from_hex(r, "F27EA91903E16641CB1465F4");

    big_int_pow(b, b, e, q);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Base close to max size & trigger rare case in div_rem
    big_int_create_from_hex(b,
        "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD");
    big_int_create_from_chunk(e, 5, 0);
    big_int_create_from_hex(q,
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    big_int_create_from_hex(r,
        "7C9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");

    big_int_pow(b, b, e, q);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Test exponent 0
    big_int_create_from_hex(b, "67AFE4589B");
    big_int_create_from_chunk(e, 0, 0);
    big_int_create_from_hex(q, "987654321ABC");
    big_int_create_from_chunk(r, 1, 0);

    big_int_pow(b, b, e, q);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Test base 1
    big_int_create_from_chunk(b, 1, 0);
    big_int_create_from_hex(e, "67AFE4589BABCD");
    big_int_create_from_hex(q, "987654321ABC");
    big_int_create_from_chunk(r, 1, 0);

    big_int_pow(b, b, e, q);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Odd power
    big_int_create_from_hex(b, "ABCDEF123456789");
    big_int_create_from_hex(e, "89BABC1");
    big_int_create_from_hex(q, "987654321ABC");
    big_int_create_from_hex(r, "1b298135d355");

    big_int_pow(b, b, e, q);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Even power
    big_int_create_from_hex(b, "ABCDEF123456789");
    big_int_create_from_hex(e, "89BABC0");
    big_int_create_from_hex(q, "987654321ABC");
    big_int_create_from_hex(r, "cbffc2df1bd");

    big_int_pow(b, b, e, q);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(e);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(r);
}
END_TEST

/**
* \brief Test finding the greatest common divisor of BigInts
*/
START_TEST(test_gcd)
{
    TEST_BIG_INT_DEFINE(a);
    TEST_BIG_INT_DEFINE(b);
    TEST_BIG_INT_DEFINE(g_exp);
    TEST_BIG_INT_DEFINE(x_exp);
    TEST_BIG_INT_DEFINE(y_exp);
    EgcdResult res;

    big_int_create_from_hex(a, "F18A8F06266D15799D7FD7C1B");
    big_int_create_from_hex(b, "31A9B6D0509E9");
    big_int_create_from_chunk(g_exp, 5, 0);
    big_int_create_from_hex(x_exp, "-2235AFC88A8C");
    big_int_create_from_hex(y_exp, "A661FAF8D8580423D4219FE1");

    big_int_egcd(&res, a, b);
    TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp);

    // Coprimes (gcd = 1)
    big_int_create_from_chunk(a, 2, 0);
    big_int_create_from_chunk(b, 3, 0);
    big_int_create_from_chunk(g_exp, 1, 0);
    big_int_create_from_chunk(x_exp, 1, 1);
    big_int_create_from_chunk(y_exp, 1, 0);

    big_int_egcd(&res, a, b);
    TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp);

    // a = b
    big_int_create_from_hex(a, "ABCDEF123456789ABCDEF");
    big_int_create_from_hex(b, "ABCDEF123456789ABCDEF");
    big_int_create_from_hex(g_exp, "ABCDEF123456789ABCDEF");
    big_int_create_from_chunk(x_exp, 1, 0);
    big_int_create_from_chunk(y_exp, 0, 0);

    big_int_egcd(&res, a, b);
    TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp);

    // Zero in 1st argument
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_hex(b, "ABCDEF123456789ABCDEF");
    big_int_create_from_hex(g_exp, "ABCDEF123456789ABCDEF");
    big_int_create_from_chunk(x_exp, 0, 0);
    big_int_create_from_chunk(y_exp, 1, 0);

    big_int_egcd(&res, a, b);
    TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp);

    // Zero in 2nd argument
    big_int_create_from_hex(a, "ABCDEF123456789ABCDEF");
    big_int_create_from_chunk(b, 0, 0);
    big_int_create_from_hex(g_exp, "ABCDEF123456789ABCDEF");
    big_int_create_from_chunk(x_exp, 1, 0);
    big_int_create_from_chunk(y_exp, 0, 0);

    big_int_egcd(&res, a, b);
    TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp);

    // Both zero arguments
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_chunk(b, 0, 0);
    big_int_create_from_chunk(g_exp, 0, 0);
    big_int_create_from_chunk(x_exp, 0, 0);
    big_int_create_from_chunk(y_exp, 0, 0);

    big_int_egcd(&res, a, b);
    TEST_BIG_INT_CMP_GCD_RES(res, g_exp, x_exp, y_exp);

    TEST_BIG_INT_DESTROY(a);
    TEST_BIG_INT_DESTROY(b);
    TEST_BIG_INT_DESTROY(g_exp);
    TEST_BIG_INT_DESTROY(x_exp);
    TEST_BIG_INT_DESTROY(y_exp);
}
END_TEST

/**
* \brief Test chi function on BigInts
*/
START_TEST(test_chi)
{
    TEST_BIG_INT_DEFINE(r);
    TEST_BIG_INT_DEFINE(t);
    TEST_BIG_INT_DEFINE(q);
    TEST_BIG_INT_DEFINE(s);
    TEST_BIG_INT_DEFINE(u);
    TEST_BIG_INT_DEFINE(v);
    TEST_BIG_INT_DEFINE(a);

    // Basic tests. q = 7
    // t is 0
    big_int_create_from_chunk(t, 0, 0);
    big_int_create_from_chunk(q, 7, 0);
    big_int_create_from_chunk(r, 0, 0);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // non-zero square
    big_int_create_from_chunk(t, 16, 0);
    big_int_create_from_chunk(q, 7, 0);
    big_int_create_from_chunk(r, 1, 0);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // non-square
    big_int_create_from_chunk(t, 17, 0);
    big_int_create_from_chunk(q, 7, 0);
    big_int_create_from_chunk(r, 1, 1);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // -1 is not a square
    big_int_create_from_chunk(t, 1, 1);
    big_int_create_from_chunk(q, 7, 0);
    big_int_create_from_chunk(r, 1, 1);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // Tests on bigger numbers: q = 2^61 - 1
    big_int_create_from_chunk(t, 0, 0);
    big_int_create_from_hex(q, "1fffffffffffffff");
    big_int_create_from_chunk(r, 0, 0);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    big_int_create_from_hex(t, "3626229738a3b9"); // 0x75bcd15 squared
    big_int_create_from_hex(q, "1fffffffffffffff");
    big_int_create_from_chunk(r, 1, 0);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // negative numbers cannot be square
    big_int_create_from_hex(t, "-3626229738a3b9");
    big_int_create_from_hex(q, "1fffffffffffffff");
    big_int_create_from_chunk(r, 1, 1);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    big_int_create_from_chunk(t, 1, 1);
    big_int_create_from_hex(q, "1fffffffffffffff");
    big_int_create_from_chunk(r, 1, 1);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    /* Advanced tests (as specified in the paper).
     * Most of these are actually pretty trivial.
     */
    // chi(chi(t)) = chi(t)
    big_int_create_from_hex(t, "3626229738A3B8");
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");
    big_int_create_from_chunk(r, 1, 1);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    big_int_create_from_chunk(t, 0, 0);
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");
    big_int_create_from_chunk(r, 0, 0);

    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    big_int_create_from_hex(t, "3626229738A3B9");
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");
    big_int_create_from_chunk(r, 1, 0);

    big_int_chi(t, t, q);
    big_int_chi(t, t, q);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // chi(st) = chi(s) * chi(t)
    // s and t both square
    big_int_create_from_hex(s, "3626229738A3B9"); // 0x75bcd15 squared
    big_int_create_from_hex(t, "734CC30B14564B142BFAFAAF71"); // ABCDEF1234567 squared
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");

    big_int_mul(a, s, t);
    big_int_chi(r, a, q); // chi(st)

    big_int_chi(u, s, q); // chi(s)
    big_int_chi(v, t, q); // chi(t)
    big_int_mul(t, u, v); // chi(s) * chi(t);

    ck_assert_int_eq(big_int_compare(r, t), 0);

    // s square, t non-square
    big_int_create_from_hex(s, "3626229738A3B9"); // 0x75bcd15 squared
    big_int_create_from_hex(t, "734CC30B14564B142BFAFAAF70");
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");

    big_int_mul(a, s, t);
    big_int_chi(r, a, q); // chi(st)

    big_int_chi(u, s, q); // chi(s)
    big_int_chi(v, t, q); // chi(t)
    big_int_mul(t, u, v); // chi(s) * chi(t);

    ck_assert_int_eq(big_int_compare(r, t), 0);

    // chi(st) = chi(s) * chi(t) for s or t = 0 is trivial.

    // chi(1/t) = chi(t) = 1/chi(t) if t != 0
    // NOTE: I don't really understand the chi(1/t) part since chi is defined for integers only?
    big_int_create_from_hex(t, "3626229738A3B9"); // 0x75bcd15 squared
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");

    big_int_chi(u, t, q); // chi(t)

    big_int_div(v, big_int_one, u); // 1/chi(t)

    ck_assert_int_eq(big_int_compare(u, v), 0);

    big_int_create_from_hex(t, "3626229738A3B8");
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");

    big_int_chi(u, t, q); // chi(t)

    big_int_div(v, big_int_one, u); // 1/chi(t)

    ck_assert_int_eq(big_int_compare(u, v), 0);

    // chi(t^2) = 1 if t != 0
    big_int_create_from_hex(t, "ABCDEF123456789");
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");
    big_int_create_from_chunk(r, 1, 0);

    big_int_mul(a, t, t); // t^2
    big_int_chi(t, a, q); // chi(t^2)
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // chi(t)t = t if t is square
    big_int_create_from_hex(t, "3626229738A3B9"); // 0x75bcd15 squared
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");

    big_int_chi(r, t, q); // chi(t)
    big_int_mul(a, r, t); // chi(t) * t
    ck_assert_int_eq(big_int_compare(t, a), 0);

    // Any square root s of t satisfies s = chi(s) * t^((q + 1)/4)
    /* TODO: Maybe I misunderstood this part of the paper,
     * but I don't see how this can hold. If s is not square
     * then it means s = -s.
     */
    big_int_create_from_hex(t, "3626229738A3B9"); // 0x75bcd15 squared
    big_int_create_from_hex(s, "75BCD15");
    big_int_create_from_hex(q, "1FFFFFFFFFFFFFFF");

    big_int_chi(u, r, q); // chi(s)

    big_int_srl_small(v, big_int_add(a, q, big_int_one), 2); // (q + 1)/4
    big_int_pow(r, t, u, q); // t^((q + 1)/4)
    big_int_mul(a, r, u); // chi(s) * t^((q+1)/4)

    // ck_assert_int_eq(big_int_compare(s, r), 0);

    TEST_BIG_INT_DESTROY(r);
    TEST_BIG_INT_DESTROY(t);
    TEST_BIG_INT_DESTROY(q);
    TEST_BIG_INT_DESTROY(s);
    TEST_BIG_INT_DESTROY(u);
    TEST_BIG_INT_DESTROY(v);
    TEST_BIG_INT_DESTROY(a);
}
END_TEST


Suite *bigints_suite(void)
{
    Suite *s;
    TCase *tc_create, *tc_basic_arith, *tc_shifts, *tc_modular_arith,
        *tc_advanced_ops;

    s = suite_create("BigInt Test Suite");

    tc_create = tcase_create("Create BigInts");
    tc_basic_arith = tcase_create("Basic Arithmetic");
    tc_shifts = tcase_create("Shifts");
    tc_modular_arith = tcase_create("Modular Arithmetic");
    tc_advanced_ops = tcase_create("Advanced Operations");

    tcase_add_test(tc_create, test_create_from_int64);
    tcase_add_test(tc_create, test_create_from_hex);

    tcase_add_test(tc_basic_arith, test_negate);
    tcase_add_test(tc_basic_arith, test_addition);
    tcase_add_test(tc_basic_arith, test_subtraction);
    tcase_add_test(tc_basic_arith, test_multiplication);
#if VERSION > 1
    tcase_add_test(tc_basic_arith, test_multiplication_single_chunk);
    tcase_add_test(tc_basic_arith, test_squared);
#endif
    tcase_add_test(tc_basic_arith, test_division);

    tcase_add_test(tc_shifts, test_sll_small);
    tcase_add_test(tc_shifts, test_srl_small);

    tcase_add_test(tc_modular_arith, test_modulo_operation);
    tcase_add_test(tc_modular_arith, test_modulo_inverse);
    tcase_add_test(tc_modular_arith, test_add_mod);
    tcase_add_test(tc_modular_arith, test_sub_mod);
    tcase_add_test(tc_modular_arith, test_mul_mod);
    tcase_add_test(tc_modular_arith, test_div_mod);

    tcase_add_test(tc_advanced_ops, test_power);
    tcase_add_test(tc_advanced_ops, test_gcd);
    tcase_add_test(tc_advanced_ops, test_chi);

    tcase_add_test(tc_basic_arith, test_comparison);

    suite_add_tcase(s, tc_create);
    suite_add_tcase(s, tc_basic_arith);
    suite_add_tcase(s, tc_shifts);
    suite_add_tcase(s, tc_modular_arith);
    suite_add_tcase(s, tc_advanced_ops);

    return s;
}

 int main(void)
 {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = bigints_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }

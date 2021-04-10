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


/**
* \brief Test creating a BigInt from an int64_t
*/
START_TEST(test_create_from_int64)
{
    BigInt *a;
    int64_t t;

    // Test positive integer
    t = 12390481;
    a = big_int_create(NULL, t);
    ck_assert_uint_eq(a->chunks[0], t);
    ck_assert_uint_eq(a->sign, 0);

    // Test negative integer
    t = -3412123;
    big_int_create(a, t);
    ck_assert_uint_eq(a->chunks[0], llabs(t));
    ck_assert_uint_eq(a->sign, 1);

    big_int_destroy(a);
}
END_TEST


/**
* \brief Test creating a BigInt from a hex string
*/
START_TEST(test_create_from_hex)
{
    BigInt *a;

    // Test positive integer
    a = big_int_create_from_hex(NULL, "DEADBEEF");
    ck_assert_uint_eq(a->chunks[0], 3735928559);
    ck_assert_uint_eq(a->sign, 0);

    // Test negative integer
    big_int_create_from_hex(a, "-C0FFEE");
    ck_assert_uint_eq(a->chunks[0], 12648430);
    ck_assert_uint_eq(a->sign, 1);

    // Test multi-chunk positive integer
    big_int_create_from_hex(a, "F050000000000000000F00D");
    ck_assert_uint_eq(a->chunks[0], 61453);
    ck_assert_uint_eq(a->chunks[1], 0);
    ck_assert_uint_eq(a->chunks[2], 251985920);
    ck_assert_uint_eq(a->size, 3);
    ck_assert_uint_eq(a->sign, 0);

    // Test multi-chunk negative integer
    big_int_create_from_hex(a, "-F050000000000000000F00D");
    ck_assert_uint_eq(a->chunks[0], 61453);
    ck_assert_uint_eq(a->chunks[1], 0);
    ck_assert_uint_eq(a->chunks[2], 251985920);
    ck_assert_uint_eq(a->size, 3);
    ck_assert_uint_eq(a->sign, 1);

    big_int_destroy(a);
}
END_TEST


/**
* \brief Test negating BigInt by flipping the sign twice, and checking that we
* arrive at the same number again
*/
START_TEST(test_negate)
{
    BigInt *a, *a_neg;

    a = big_int_create(NULL, 123);

    // Flip 1
    a_neg = big_int_neg(NULL, a);
    // TODO: change to big_int_compare once implemented
    ck_assert_uint_eq(a->chunks[0], a_neg->chunks[0]);
    ck_assert_uint_ne(a->sign, a_neg->sign);

    // Flip 2
    big_int_neg(a_neg, a_neg);
    // TODO: change to big_int_compare once implemented
    ck_assert_uint_eq(a->chunks[0], a_neg->chunks[0]);
    ck_assert_uint_eq(a->sign, a_neg->sign);

    big_int_destroy(a);
    big_int_destroy(a_neg);
}


/**
* \brief Test addition of BigInts
*/
START_TEST(test_addition)
{
    BigInt *a, *b, *r;

    // normal addition of two positive integers
    a = big_int_create(NULL, 123);
    b = big_int_create(NULL, 123412);
    r = big_int_create(NULL, 123535);

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // normal addition of a positive and a negative integers
    big_int_create(a, 123);
    big_int_create(b, -123412);
    big_int_create(r, -123289);

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // TODO: normal addition of a negative and a positive integers
    // TODO: normal addition of two negative integers
    // TODO: multi-chunk addition of two positive integers
    // TODO: multi-chunk addition of one positive integer and one negative integer
    // TODO: multi-chunk addition of one negative integer and one positive integer
    // TODO: multi-chunk addition of two negative integers

    big_int_destroy(a);
    big_int_destroy(b);
    big_int_destroy(r);
}

// TODO: test subtraction
// TODO: test comparison

Suite *basic_arith_suite(void)
{
    Suite *s;
    TCase *tc_basic_arith;

    s = suite_create("BigInt Test Suite");

    tc_basic_arith = tcase_create("Basic Arithmetic");

    tcase_add_test(tc_basic_arith, test_create_from_int64);
    tcase_add_test(tc_basic_arith, test_create_from_hex);
    tcase_add_test(tc_basic_arith, test_negate);
    tcase_add_test(tc_basic_arith, test_addition);

    suite_add_tcase(s, tc_basic_arith);

    return s;
}

 int main(void)
 {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = basic_arith_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }

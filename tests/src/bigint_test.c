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
* \brief Test basic arithmetic operations
*/
// TODO: Test multi-chunk addition, maybe use Python as "golden model"
// START_TEST(test_simple_add)
// {
//     BigInt *a, *b, *r;
//
//     a = big_int_create(13);
//     b = big_int_create(37);
//
//     r = big_int_add(a, b);
//
//     ck_assert_uint_eq(r->chunks[0], 50);
//     ck_assert_uint_eq(r->sign, 0);
//
//     big_int_destroy(a);
//     big_int_destroy(b);
//     big_int_destroy(r);
// }
// END_TEST


/**
* \brief Test creating a BigInt from an int64_t
*/
START_TEST(test_create_from_int64)
{
    BigInt *a;
    int64_t t;

    // Test positive integer
    t = 12390481;
    a = big_int_create(t);
    ck_assert_uint_eq(a->chunks[0], t);
    ck_assert_uint_eq(a->sign, 0);
    big_int_destroy(a);

    // Test negative integer
    t = -3412123;
    a = big_int_create(t);
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
    a = big_int_create_from_hex("DEADBEEF");
    ck_assert_uint_eq(a->chunks[0], 3735928559);
    ck_assert_uint_eq(a->sign, 0);
    big_int_destroy(a);

    // Test negative integer
    a = big_int_create_from_hex("-C0FFEE");
    ck_assert_uint_eq(a->chunks[0], 12648430);
    ck_assert_uint_eq(a->sign, 1);
    big_int_destroy(a);

    // Test multi-chunk positive integer
    a = big_int_create_from_hex("F050000000000000000f00d");
    ck_assert_uint_eq(a->chunks[0], 61453);
    ck_assert_uint_eq(a->chunks[1], 251985920);
    ck_assert_uint_eq(a->sign, 0);
    big_int_destroy(a);

    // Test multi-chunk negative integer
    a = big_int_create_from_hex("-F050000000000000000f00d");
    ck_assert_uint_eq(a->chunks[0], 61453);
    ck_assert_uint_eq(a->chunks[1], 251985920);
    ck_assert_uint_eq(a->sign, 1);
    big_int_destroy(a);
}
END_TEST


Suite *basic_arith_suite(void)
{
    Suite *s;
    TCase *tc_basic_arith;

    s = suite_create("BigInt Test Suite");

    tc_basic_arith = tcase_create("Basic Arithmetic");

    // tcase_add_test(tc_basic_arith, test_simple_add);
    tcase_add_test(tc_basic_arith, test_create_from_int64);
    tcase_add_test(tc_basic_arith, test_create_from_hex);

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

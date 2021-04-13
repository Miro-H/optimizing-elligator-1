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
    ck_assert_uint_eq(a->chunks[0], a_neg->chunks[0]);
    ck_assert_uint_ne(a->sign, a_neg->sign);

    // Flip 2
    big_int_neg(a_neg, a_neg);
    ck_assert_int_eq(big_int_compare(a, a_neg), 0);
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

    // addition with single chunk overflow
    big_int_create(a, 4294967290);
    big_int_create(b, 16);
    big_int_create_from_hex(r, "10000000A");

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // normal addition of a positive and a negative integers
    big_int_create(a, 123);
    big_int_create(b, -123412);
    big_int_create(r, -123289);

    big_int_add(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // normal addition of a negative and a positive integers
    big_int_create(a, -89545823);
    big_int_create(b, 89545823);

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, big_int_zero);
    ck_assert_int_eq(big_int_compare(a, big_int_zero), 0);

    // normal addition of a negative and a positive integers
    big_int_create(a, -4);
    big_int_create(b, -2);
    big_int_create(r, -6);

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, r);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // addition with single chunk underflow
    big_int_create(a, -4294967295);
    big_int_create(b, -2);
    big_int_create_from_hex(r, "-100000001");

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, big_int_zero);
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // TODO: multi-chunk addition of two positive integers
    big_int_create_from_hex(a, "A20B9BDB69E6C331825D79743C398D0E");
    big_int_create_from_hex(b, "293D794457EA9BCA15E3E286B3998176");
    big_int_create_from_hex(r, "CB49151FC1D15EFB98415BFAEFD30E84");

    big_int_add(a, a, b); // a = a + b
    big_int_compare(a, r);
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // TODO: multi-chunk addition of one positive integer and one negative integer
    // TODO: multi-chunk addition of one negative integer and one positive integer
    // TODO: multi-chunk addition of two negative integers
    // TODO: test mixed sized BigInt ops

    // TODO (low prio): test 256 overflows -> restriction of fixed size for bigints,
    // but that should never occur in modulo ops.

    big_int_destroy(a);
    big_int_destroy(b);
    big_int_destroy(r);
}

/**
* \brief Test subtractions of BigInts
*/
START_TEST(test_subtraction)
{
    BigInt *a, *b, *r;

    // normal subtraction of two positive chunks
    a = big_int_create(NULL, 123);
    b = big_int_create(NULL, 123412);
    r = big_int_create(NULL, -123289);

    big_int_sub(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // TODO: normal subtraction of two negative chunks

    // multi-chunk subtraction of two positive numbers
    big_int_create_from_hex(a, "BA2980E4A996ED0AAEA5B0E3B65A7048");
    big_int_create_from_hex(b, "531FEC5ED503B8D3");
    big_int_create_from_hex(r, "BA2980E4A996ED0A5B85C484E156B775");

    big_int_sub(a, a, b); // a = a + b
    ck_assert_int_eq(big_int_compare(a, r), 0);
    ck_assert_uint_eq(a->overflow, 0);

    // TODO: multi-chunk subtraction of two negative numbers
    // TODO: mixed signs (uses addition underneath, but test pos/neg and neg/pos)
    // TODO: test chunk overflow
    // TODO: test chunk underflow

    big_int_destroy(a);
    big_int_destroy(b);
    big_int_destroy(r);
}

// TODO: test comparison
// TODO: test +0 and -0 comparisons
// TODO: test a < b
// TODO: test a > b
// TODO: test different cases (see bigint.c) for same/diff signs and same/diff sizes

START_TEST(test_multiplication)
{
    BigInt *a, *b, *r;

    // single chunk multiplication
    a = big_int_create(NULL, 31180);
    b = big_int_create(NULL, 22299);
    r = big_int_create(NULL, 695282820);

    big_int_mul(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // multi-chunk multiplication
    big_int_create_from_hex(a, "859CC30B50220F05");
    big_int_create_from_hex(b, "-F623FE531DA1D2B3");
    big_int_create_from_hex(r, "-80776C98745C5CEA986DBE62FB479A7F");

    big_int_mul(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Zero multiplication
    big_int_create(a, 0);
    big_int_create_from_hex(b, "-F623FE531DA1D2B3");

    big_int_mul(a, a, b);
    ck_assert_int_eq(big_int_compare(a, big_int_zero), 0);

    // Multiplications with zero chunks
    big_int_create_from_hex(a, "859CC30B00000000");
    big_int_create_from_hex(b, "F623FE531DA1D2B3");
    big_int_create_from_hex(r, "80776C9827505E37ED8666B100000000");

    big_int_mul(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // TODO: test multiply integers of different sizes
    // TODO: what other things can we test?

    big_int_destroy(a);
    big_int_destroy(b);
    big_int_destroy(r);
}

START_TEST(test_division)
{
    BigInt *a, *b, *q, *r_exp, *q_exp, *r;

    r = big_int_create(NULL, 0);
    q = big_int_create(NULL, 0);

    // single chunk division
    a = big_int_create(NULL, 4184080774);
    b = big_int_create(NULL, 9961738);
    r_exp = big_int_create(NULL, 150814);
    q_exp = big_int_create(NULL, 420);

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

    // TODO: Definitely test division more in-depth - I don't really trust the
    //       code that much.

    // TODO: test special case marked in big_int_div_rem in a TODO and mentioned
    //       specifically in the book (last link in git issue).

    // TODO: This test seems to trigger some bug related to D5
    // big_int_create_from_hex(a, "5F9F737BB0C9873450C3A516A8B5F0F0");
    // big_int_create_from_hex(b, "68A753D72F1F907F1BFAF91F0");
    // big_int_create_from_hex(q_exp, "E9E8C50");
    // big_int_create_from_hex(r_exp, "185E80D262E90C5F8BE915F0");
    //
    // big_int_div_rem(q, r, a, b);
    // DEBUG_BIGINT(q, "Got q = ");
    // DEBUG_BIGINT(r, "Got r = ");
    // ck_assert_int_eq(big_int_compare(q_exp, q), 0);
    // ck_assert_int_eq(big_int_compare(r_exp, r), 0);

    big_int_destroy(a);
    big_int_destroy(b);
    big_int_destroy(q);
    big_int_destroy(r);
    big_int_destroy(r_exp);
}

START_TEST(test_sll_small)
{
    BigInt *a, *r;

    // TODO: Test big_int_sll_small with more cases

    // Shift over chunk border
    a = big_int_create(NULL, 4184080774);
    r = big_int_create_from_hex(NULL, "3e590061800");

    big_int_sll_small(a, a, 10);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_destroy(a);
    big_int_destroy(r);
}

START_TEST(test_srl_small)
{
    BigInt *a, *r;

    // TODO: Test big_int_srl_small with more cases

    // Shift over chunk border
    a = big_int_create_from_hex(NULL, "102CC2711CB04A427");
    r = big_int_create_from_hex(NULL, "102CC2711CB04A42");

    big_int_srl_small(a, a, 4);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_destroy(a);
    big_int_destroy(r);
}


START_TEST(test_modulo_operation)
{
    BigInt *a, *q, *r;

    // TODO: Test big_int_mod with more cases

    a = big_int_create_from_hex(NULL, "D94DD4B70E1C291FAE8BB971F");
    q = big_int_create_from_hex(NULL, "1034053B0A");
    r = big_int_create_from_hex(NULL, "1A8E7BE75");

    big_int_mod(a, a, q);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_destroy(a);
    big_int_destroy(q);
    big_int_destroy(r);
}

// TODO: quickly test big_int_add_mod
// TODO: quickly test big_int_sub_mod
// TODO: quickly test big_int_mul_mod
// TODO: quickly test big_int_div_mod

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
    tcase_add_test(tc_basic_arith, test_subtraction);
    tcase_add_test(tc_basic_arith, test_multiplication);
    tcase_add_test(tc_basic_arith, test_sll_small);
    tcase_add_test(tc_basic_arith, test_srl_small);
    tcase_add_test(tc_basic_arith, test_division);
    tcase_add_test(tc_basic_arith, test_modulo_operation);

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

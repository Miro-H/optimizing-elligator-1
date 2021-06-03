/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This is a unit test file to test the GMP Elligator implementation.
 */

/*
 * Includes
 */
#include <check.h>
#include <stdlib.h>

// Include header files
#include "gmp_elligator.h"

/**
 * \brief Test chi function on mpz_t
 */
START_TEST(test_chi)
{
    mpz_t r, t, q, s, u, v, one;

    mpz_init(r);
    mpz_init(t);
    mpz_init(q);
    mpz_init(s);
    mpz_init(u);
    mpz_init(v);
    mpz_init(one);

    // Basic tests. q = 7
    // t is 0
    mpz_set_si(t, 0);
    mpz_set_si(q, 7);
    mpz_set_si(r, 0);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // non-zero square
    mpz_set_si(t, 16);
    mpz_set_si(q, 7);
    mpz_set_si(r, 1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // non-square
    mpz_set_si(t, 17);
    mpz_set_si(q, 7);
    mpz_set_si(r, -1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // -1 is not a square
    mpz_set_si(t, -1);
    mpz_set_si(q, 7);
    mpz_set_si(r, -1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // Tests on bigger numbers: q = 2^61 - 1
    mpz_set_si(t, 0);
    mpz_set_str(q, "1fffffffffffffff", 16);
    mpz_set_si(r, 0);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    mpz_set_str(t, "3626229738a3b9", 16); // 0x75bcd15 squared
    mpz_set_str(q, "1fffffffffffffff", 16);
    mpz_set_si(r, 1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // negative numbers cannot be square
    mpz_set_str(t, "-3626229738a3b9", 16);
    mpz_set_str(q, "1fffffffffffffff", 16);
    mpz_set_si(r, -1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    mpz_set_si(t, -1);
    mpz_set_str(q, "1fffffffffffffff", 16);
    mpz_set_si(r, -1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // Advanced tests (as specified in the paper)
    // chi(chi(t)) = chi(t)
    mpz_set_str(t, "3626229738A3B8", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);
    mpz_set_si(r, -1);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    mpz_set_si(t, 0);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);
    mpz_set_si(r, 0);

    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    mpz_set_str(t, "3626229738A3B9", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);
    mpz_set_si(r, 1);

    mpz_chi(t, t, q);
    mpz_chi(t, t, q);
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // chi(st) = chi(s) * chi(t)
    // s and t both square
    mpz_set_str(s, "3626229738A3B9", 16); // 0x75bcd15 squared
    mpz_set_str(t, "734CC30B14564B142BFAFAAF71", 16); // ABCDEF1234567 squared
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);

    mpz_set(r, s);
    mpz_mul(r, r, t);
    mpz_chi(r, r, q); // chi(st)

    mpz_set(u, s);
    mpz_chi(u, u, q); //chi(s)

    mpz_set(v, t);
    mpz_chi(v, v, q); // chi(t)

    mpz_mul(u, u, v); // chi(s) * chi(t)

    ck_assert_int_eq(mpz_cmp(r, u), 0);

    // s square, t non-square
    mpz_set_str(s, "3626229738A3B9", 16);
    mpz_set_str(t, "734CC30B14564B142BFAFAAF70", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);

    mpz_set(r, s);
    mpz_mul(r, r, t);
    mpz_chi(r, r, q); // chi(st)

    mpz_set(u, s);
    mpz_chi(u, u, q); // chi(s)

    mpz_set(v, t);
    mpz_chi(v, v, q); // chi(t)

    mpz_mul(u, u, v); // chi(s) * chi(t)

    ck_assert_int_eq(mpz_cmp(r, u), 0);

    // chi(1/t) = chi(t) = 1/chi(t) if t != 0
    mpz_set_str(t, "3626229738A3B9", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);

    mpz_chi(t, t, q); // chi(t)

    mpz_set(r, t);
    mpz_set_si(one, 1);
    mpz_fdiv_q(r, one, t); // 1/chi(t)

    ck_assert_int_eq(mpz_cmp(t, r), 0);

    mpz_set_str(t, "3626229738A3B8", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);

    mpz_chi(t, t, q); // chi(t)

    mpz_set(r, t);
    mpz_fdiv_q(r, one, t); // 1/chi(t)

    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // chi(t^2) = 1 if t != 0
    mpz_set_str(t, "ABCDEF123456789", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);
    mpz_set_si(r, 1);

    mpz_mul(t, t, t); // t^2
    mpz_chi(t, t, q); // chi(t^2)
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    // chi(t)t = t if t is square
    mpz_set_str(t, "3626229738A3B9", 16);
    mpz_set_str(q, "1FFFFFFFFFFFFFFF", 16);

    mpz_set(r, t);
    mpz_chi(r, t, q); // chi(t)
    mpz_mul(r, r, t); // chi(t) * t
    ck_assert_int_eq(mpz_cmp(t, r), 0);

    mpz_clear(r);
    mpz_clear(t);
    mpz_clear(q);
    mpz_clear(s);
    mpz_clear(u);
    mpz_clear(v);
    mpz_clear(one);
}
END_TEST

Suite *gmp_elligator_suite(void)
{
    Suite *s;
    TCase *tc_chi;

    s = suite_create("GMP Elligator Test Suite");

    tc_chi = tcase_create("Chi function");

    tcase_add_test(tc_chi, test_chi);

    suite_add_tcase(s, tc_chi);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = gmp_elligator_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
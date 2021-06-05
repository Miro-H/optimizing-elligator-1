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

/**
 * \brief Check Curve1174 constants
 */
START_TEST(test_curve1174)
{
    GMP_Curve curve;
    mpz_t c, d, r;

    mpz_init(c);
    mpz_init(d);
    mpz_init(r);

    gmp_init_curve1174(&curve);

    mpz_set_str(c,
        "4D1A3398ED42CEEB451D20824CA9CB49B69EF546BD7E6546AEF19AF1F9E49E1",
        16);
    mpz_set_str(d,
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB61",
        16);
    mpz_set_str(r,
        "6006FBDA7649C433816B286006FBDA7649C433816B286006FBDA7649C43383",
        16);

    ck_assert_int_eq(mpz_cmp(curve.c, c), 0);
    ck_assert_int_eq(mpz_cmp(curve.d, d), 0);
    ck_assert_int_eq(mpz_cmp(curve.r, r), 0);

    mpz_clear(c);
    mpz_clear(d);
    mpz_clear(r);

    gmp_free_curve(&curve);
}
END_TEST

/**
 * \brief Test mapping a number to the curve and back
 */
START_TEST(test_e2e)
{
    GMP_Curve curve;
    GMP_CurvePoint curve_point;
    mpz_init(curve_point.x);
    mpz_init(curve_point.y);

    mpz_t r, t, x, y;

    mpz_init(r);
    mpz_init(t);
    mpz_init(x);
    mpz_init(y);

    gmp_init_curve1174(&curve);

    mpz_set_si(t, 7);
    mpz_set_str(x,
        "AB65983CF55A18C0E2C8BB8A156E030566D23767D6C1473ACFCF4D17439AC7",
        16);
    mpz_set_str(y,
        "49C01F8D8C86ECB362B3952FA93ABD8CF512B09225BCEE9E76BC5E0C9A6E17E",
        16);

    // Map BigInt to curve point
    gmp_elligator_1_string_to_point(&curve_point, t, curve);
    ck_assert_int_eq(mpz_cmp(curve_point.x, x), 0);
    ck_assert_int_eq(mpz_cmp(curve_point.y, y), 0);

    // Map curve point back to mpz_t
    gmp_elligator_1_point_to_string(r, curve_point, curve);
    ck_assert_int_eq(mpz_cmp(r, t), 0);

    // Do the same with more complex inputs
    mpz_set_str(t, "75BCD15", 16);
    mpz_set_str(x,
        "1A47D90E9F9C19AB846C9E100317F693607A1D16851FD05B40E7DA6FFF5BCF",
        16);
    mpz_set_str(y,
        "5BBE4619CEA4729F94082B429693AC4B565F94CDA5D6D875689DE765C19A461",
        16);

    gmp_elligator_1_string_to_point(&curve_point, t, curve);
    ck_assert_int_eq(mpz_cmp(curve_point.x, x), 0);
    ck_assert_int_eq(mpz_cmp(curve_point.y, y), 0);

    // Map curve point back to mpz_t
    gmp_elligator_1_point_to_string(r, curve_point, curve);
    ck_assert_int_eq(mpz_cmp(r, t), 0);

    mpz_set_str(t,
        "ABCDEF1234567899987654321ABCABCDEFDEF",
        16);
    mpz_set_str(x,
        "7D4DD83C5E1F4E2DBC79CA0941102DE6376604C9D8A75EB41B6875010211B50",
        16);
    mpz_set_str(y,
        "61C507D563235BEA439C1D53BDDF33E0D87C0041D3228CD0CA00C3D2A59025E",
        16);

    gmp_elligator_1_string_to_point(&curve_point, t, curve);
    ck_assert_int_eq(mpz_cmp(curve_point.x, x), 0);
    ck_assert_int_eq(mpz_cmp(curve_point.y, y), 0);

    // Map curve point back to mpz_t
    gmp_elligator_1_point_to_string(r, curve_point, curve);
    ck_assert_int_eq(mpz_cmp(r, t), 0);

    mpz_set_si(t, 2);
    mpz_set_str(x,
        "6F5374156B145FF8BB3288E0418F513B5D7BBBAB6E252EA1BC2DB6428E1454E",
        16);
    mpz_set_str(y,
        "ED7F6014F111318ED7F6014F111318ED7F6014F111318ED7F6014F111318EC",
        16);

    gmp_elligator_1_string_to_point(&curve_point, t, curve);
    ck_assert_int_eq(mpz_cmp(curve_point.x, x), 0);
    ck_assert_int_eq(mpz_cmp(curve_point.y, y), 0);

    // Map curve point back to mpz_t
    gmp_elligator_1_point_to_string(r, curve_point, curve);
    ck_assert_int_eq(mpz_cmp(r, t), 0);

    mpz_clear(t);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(r);

    gmp_free_curve_point(&curve_point);
    gmp_free_curve(&curve);
}
END_TEST

/**
 * \brief Test edge cases for Elligator mapping
 */
START_TEST(test_edge_cases)
{
    GMP_Curve curve;
    GMP_CurvePoint curve_point;

    mpz_t r, t, x, y;

    mpz_init(curve_point.x);
    mpz_init(curve_point.y);
    mpz_init(r);
    mpz_init(t);
    mpz_init(x);
    mpz_init(y);

    // t = 1
    gmp_init_curve1174(&curve);

    mpz_set_si(t, 1);
    mpz_set_si(x, 0);
    mpz_set_si(y, 1);

    gmp_elligator_1_string_to_point(&curve_point, t, curve);
    ck_assert_int_eq(mpz_cmp(curve_point.x, x), 0);
    ck_assert_int_eq(mpz_cmp(curve_point.y, y), 0);

    gmp_elligator_1_point_to_string(r, curve_point, curve);
    ck_assert_int_eq(mpz_cmp(r, t), 0);

    mpz_clear(t);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(r);

    gmp_free_curve_point(&curve_point);
    gmp_free_curve(&curve);
}
END_TEST

/**
 * \brief Tests based on statements about curve1174 in paper
 */
START_TEST(test_advanced_curve1174)
{
    GMP_Curve curve;

    mpz_t q_mod4, q_mod4_exp, temp1, temp2, inverse;

    mpz_init(q_mod4);
    mpz_init(q_mod4_exp);
    mpz_init(temp1);
    mpz_init(temp2);
    mpz_init(inverse);

    gmp_init_curve1174(&curve);

    mpz_set_si(q_mod4_exp, 3);
    mpz_set_si(q_mod4_exp, 3);
    mpz_mod_ui(q_mod4, curve.q, 4);

    ck_assert_int_eq(mpz_cmp(q_mod4, q_mod4_exp), 0);

    // c * (c - 1) * (c + 1) != 0
    mpz_set_si(temp1, 0);
    mpz_set_si(temp2, 0);
    ck_assert_int_ne(mpz_cmp_si(curve.c, 0), 0);

    mpz_sub_ui(temp1, curve.c, 1);
    ck_assert_int_ne(mpz_cmp_si(temp1, 0), 0);

    mpz_add_ui(temp2, curve.c, 1);
    ck_assert_int_ne(mpz_cmp_si(temp2, 0), 0);

    // Check that d = -1174 mod q
    mpz_set_si(temp1, -1174);
    mpz_mod(temp1, temp1, curve.q);
    ck_assert_int_eq(mpz_cmp(temp1, curve.d), 0);

    // Check that d = -(c + 1)^2 / (c - 1)^2 mod q
    mpz_set(temp1, curve.c);
    mpz_add_ui(temp1, temp1, 1);
    mpz_mul(temp1, temp1, temp1);
    mpz_mod(temp1, temp1, curve.q);
    mpz_neg(temp1, temp1);

    mpz_set(temp2, curve.c);
    mpz_sub_ui(temp2, temp2, 1);

    mpz_mul(temp2, temp2, temp2);
    mpz_mod(temp2, temp2, curve.q);
    mpz_invert(inverse, temp2, curve.q);
    mpz_mul(temp1, temp1, inverse);
    mpz_mod(temp1, temp1, curve.q);
    ck_assert_int_eq(mpz_cmp(temp1, curve.d), 0);

    // r != 0
    ck_assert_int_ne(mpz_cmp_si(curve.r, 0), 0);

    // d is not square
    mpz_chi(temp1, curve.d, curve.q);
    ck_assert_int_eq(mpz_cmp_si(temp1, -1), 0);

    mpz_clear(q_mod4);
    mpz_clear(q_mod4_exp);
    mpz_clear(temp1);
    mpz_clear(temp2);
    mpz_clear(inverse);

    gmp_free_curve(&curve);
}
END_TEST

/**
 * \brief Tests based on statements about mapping in paper
 */
START_TEST(test_advanced_string_to_point)
{
    GMP_Curve curve;
    GMP_CurvePoint curve_point;

    mpz_t t, temp1, temp2, temp3;

    mpz_init(curve_point.x);
    mpz_init(curve_point.y);
    mpz_init(t);
    mpz_init(temp1);
    mpz_init(temp2);
    mpz_init(temp3);

    gmp_init_curve1174(&curve);

    mpz_set_str(t, "ABCDEF1234567899987654321ABCABCDEFDEF", 16);

    gmp_elligator_1_string_to_point(&curve_point, t, curve);

    // x^2 + y^2 = 1 + d * x^2 * y^2 (mod q)
    mpz_mul(temp1, curve_point.x, curve_point.x);
    mpz_mod(temp1, temp1, curve.q);
    mpz_mul(temp2, curve_point.y, curve_point.y);
    mpz_mod(temp2, temp2, curve.q);
    mpz_mul(temp3, temp1, temp2);
    mpz_mod(temp3, temp3, curve.q);
    mpz_add(temp1, temp1, temp2);
    mpz_mod(temp1, temp1, curve.q);
    mpz_mul(temp3, curve.d, temp3);
    mpz_mod(temp3, temp3, curve.q);
    mpz_add_ui(temp3, temp3, 1);
    mpz_mod(temp3, temp3, curve.q);

    ck_assert_int_eq(mpz_cmp(temp1, temp3), 0);

    // Same test with t = 1
    mpz_set_si(t, 1);

    gmp_elligator_1_string_to_point(&curve_point, t, curve);

    mpz_mul(temp1, curve_point.x, curve_point.x);
    mpz_mod(temp1, temp1, curve.q);
    mpz_mul(temp2, curve_point.y, curve_point.y);
    mpz_mod(temp2, temp2, curve.q);
    mpz_mul(temp3, temp1, temp2);
    mpz_mod(temp3, temp3, curve.q);
    mpz_add(temp1, temp1, temp2);
    mpz_mod(temp1, temp1, curve.q);
    mpz_mul(temp3, curve.d, temp3);
    mpz_mod(temp3, temp3, curve.q);
    mpz_add_ui(temp3, temp3, 1);
    mpz_mod(temp3, temp3, curve.q);

    ck_assert_int_eq(mpz_cmp(temp1, temp3), 0);

    mpz_clear(t);
    mpz_clear(temp1);
    mpz_clear(temp2);
    mpz_clear(temp3);

    gmp_free_curve(&curve);
    gmp_free_curve_point(&curve_point);
}
END_TEST

Suite *gmp_elligator_suite(void)
{
    Suite *s;
    TCase *tc_chi, *tc_basic, *tc_advanced;

    s = suite_create("GMP Elligator Test Suite");

    tc_chi = tcase_create("Chi function");

    tcase_add_test(tc_chi, test_chi);

    tc_basic = tcase_create("Basic tests");

    tcase_add_test(tc_basic, test_curve1174);
    tcase_add_test(tc_basic, test_e2e);
    tcase_add_test(tc_basic, test_edge_cases);

    tc_advanced = tcase_create("Advanced Tests");
    tcase_add_test(tc_advanced, test_advanced_curve1174);
    tcase_add_test(tc_advanced, test_advanced_string_to_point);

    suite_add_tcase(s, tc_chi);
    suite_add_tcase(s, tc_basic);
    suite_add_tcase(s, tc_advanced);

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

/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This is a unit test file to test the Elligator implementation.
 */

/*
 * Includes
 */
#include <check.h>
#include <stdlib.h>

// Include header files
#include "bigint.h"
#include "elligator.h"
#include "debug.h"


/**
* \brief Test mapping a number to the curve and back
*/
START_TEST(test_curve1174)
{
    Curve curve;
    BigInt *c, *d, *r;

    c = big_int_create_from_hex(NULL,
            "4D1A3398ED42CEEB451D20824CA9CB49B69EF546BD7E6546AEF19AF1F9E49E1");
    d = big_int_create_from_hex(NULL,
            "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB61");
    r = big_int_create_from_hex(NULL,
            "6006FBDA7649C433816B286006FBDA7649C433816B286006FBDA7649C43383");

    init_curve1174(&curve);

    ck_assert_int_eq(big_int_compare(curve.c, c), 0);
    ck_assert_int_eq(big_int_compare(curve.d, d), 0);
    ck_assert_int_eq(big_int_compare(curve.r, r), 0);

    big_int_destroy(c);
    big_int_destroy(d);
    big_int_destroy(r);

    free_curve(&curve);
}

/**
* \brief Test mapping a number to the curve and back
*/
START_TEST(test_e2e)
{
    Curve curve;
    CurvePoint curve_point;
    BigInt *r, *t, *x, *y;

    init_curve1174(&curve);
    t = big_int_create(NULL, 7);
    x = big_int_create_from_hex(NULL,
            "AB65983CF55A18C0E2C8BB8A156E030566D23767D6C1473ACFCF4D17439AC7");
    y = big_int_create_from_hex(NULL,
            "49C01F8D8C86ECB362B3952FA93ABD8CF512B09225BCEE9E76BC5E0C9A6E17E");

    // Map BigInt to curve point
    curve_point = elligator_1_string_to_point(t, curve);
    ck_assert_int_eq(big_int_compare(curve_point.x, x), 0);
    ck_assert_int_eq(big_int_compare(curve_point.y, y), 0);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    // Do the same with more complex inputs
    t = big_int_create_from_hex(t, "75BCD15");
    x = big_int_create_from_hex(x,
            "1A47D90E9F9C19AB846C9E100317F693607A1D16851FD05B40E7DA6FFF5BCF");
    y = big_int_create_from_hex(y,
            "5BBE4619CEA4729F94082B429693AC4B565F94CDA5D6D875689DE765C19A461");
    curve_point = elligator_1_string_to_point(t, curve);
    ck_assert_int_eq(big_int_compare(curve_point.x, x), 0);
    ck_assert_int_eq(big_int_compare(curve_point.y, y), 0);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    t = big_int_create_from_hex(t, "ABCDEF1234567899987654321ABCABCDEFDEF");
    x = big_int_create_from_hex(x,
            "7D4DD83C5E1F4E2DBC79CA0941102DE6376604C9D8A75EB41B6875010211B50");
    y = big_int_create_from_hex(y,
            "61C507D563235BEA439C1D53BDDF33E0D87C0041D3228CD0CA00C3D2A59025E");
    curve_point = elligator_1_string_to_point(t, curve);
    ck_assert_int_eq(big_int_compare(curve_point.x, x), 0);
    ck_assert_int_eq(big_int_compare(curve_point.y, y), 0);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    t = big_int_create(t, 2);
    x = big_int_create_from_hex(x,
            "6F5374156B145FF8BB3288E0418F513B5D7BBBAB6E252EA1BC2DB6428E1454E");
    y = big_int_create_from_hex(y,
            "ED7F6014F111318ED7F6014F111318ED7F6014F111318ED7F6014F111318EC");
    curve_point = elligator_1_string_to_point(t, curve);
    ck_assert_int_eq(big_int_compare(curve_point.x, x), 0);
    ck_assert_int_eq(big_int_compare(curve_point.y, y), 0);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    // Negative t are no valid input for Elligator 1

    big_int_destroy(t);
    big_int_destroy(x);
    big_int_destroy(y);
    big_int_destroy(r);

    free_curve_point(&curve_point);
    free_curve(&curve);
}
END_TEST

/**
* \brief Test edges cases for Elligator mapping
*/
START_TEST(test_edge_cases)
{
        Curve curve;
        CurvePoint curve_point;
        BigInt *r, *t, *x, *y;

        // t = 1
        init_curve1174(&curve);
        t = big_int_create(NULL, 1);
        x = big_int_create(NULL, 0);
        y = big_int_create(NULL, 1);

        curve_point = elligator_1_string_to_point(t, curve);
        ck_assert_int_eq(big_int_compare(curve_point.x, x), 0);
        ck_assert_int_eq(big_int_compare(curve_point.y, y), 0);

        r = elligator_1_point_to_string(curve_point, curve);
        ck_assert_int_eq(big_int_compare(r, t), 0);

        // t = -1 is not a valid input for the Elligator mapping

        big_int_destroy(t);
        big_int_destroy(x);
        big_int_destroy(y);
        big_int_destroy(r);

        free_curve_point(&curve_point);
        free_curve(&curve);
}
END_TEST

/**
* \brief Tests based on statements about curve1174 in paper
*/
START_TEST(test_advanced_curve1174)
{
    Curve curve;
    BigInt *q_mod4, *q_mod4_exp, *four, *temp1, *temp2;

    init_curve1174(&curve);

    // q % 4 = 3
    q_mod4_exp = big_int_create(NULL, 3);

    q_mod4 = big_int_create(NULL, 0);
    four = big_int_create(NULL, 4);
    q_mod4 = big_int_mod(q_mod4, curve.q, four);
    ck_assert_int_eq(big_int_compare(q_mod4, q_mod4_exp), 0);

    /* c * (c - 1) * (c + 1) != 0
     * Actually calculating this with BigInts requires too many bits,
     * so I will just check that each part != 0 instead.
     */
    temp1 = big_int_create(NULL, 0);
    temp2 = big_int_create(NULL, 0);
    ck_assert_int_ne(big_int_compare(curve.c, big_int_zero), 0);

    big_int_sub(temp1, curve.c, big_int_one); // temp1 = curve.c - 1
    ck_assert_int_ne(big_int_compare(temp1, big_int_zero), 0);

    big_int_add(temp2, curve.c, big_int_one); // temp2 = curve.c + 1
    ck_assert_int_ne(big_int_compare(temp2, big_int_zero), 0);

    /*
     * Check that d = -1174 mod q
     */
    temp1 = big_int_create(temp1, -1174);
    big_int_mod(temp1, temp1, curve.q);
    ck_assert_int_eq(big_int_compare(temp1, curve.d), 0);

    // Check that d = -(c + 1)^2 / (c - 1)^2 mod q
    big_int_copy(temp1, curve.c);
    big_int_add(temp1, temp1, big_int_one);
    big_int_mul_mod(temp1, temp1, temp1, curve.q);
    big_int_neg(temp1, temp1); // temp1 = -(c + 1)^2

    big_int_copy(temp2, curve.c);
    big_int_sub(temp2, temp2, big_int_one);
    big_int_mul_mod(temp2, temp2, temp2, curve.q); // temp1 = (c - 1)^2

    big_int_div_mod(temp1, temp1, temp2, curve.q); // temp1 = -(c + 1)^2 / (c - 1)^2 mod q
    ck_assert_int_eq(big_int_compare(temp1, curve.d), 0);

    // r != 0
    ck_assert_int_ne(big_int_compare(curve.r, big_int_zero), 0);

    // d is not square
    big_int_chi(temp1, curve.d, curve.q);
    ck_assert_int_eq(big_int_compare(temp1, big_int_min_one), 0);

    big_int_destroy(q_mod4_exp);
    big_int_destroy(q_mod4);
    big_int_destroy(four);
    big_int_destroy(temp1);
    big_int_destroy(temp2);

    free_curve(&curve);
}
END_TEST

/**
* \brief Tests based on statements about mapping in paper
*/
START_TEST(test_advanced_string_to_point)
{
    Curve curve;
    CurvePoint curve_point;
    BigInt *t, *temp1, *temp2, *temp3;

    init_curve1174(&curve);
    t = big_int_create_from_hex(NULL, "ABCDEF1234567899987654321ABCABCDEFDEF");
    curve_point = elligator_1_string_to_point(t, curve);

    // x^2 + y^2 = 1 + d * x^2 * y^2 (mod q)
    temp1 = big_int_create(NULL, 0);
    temp2 = big_int_create(NULL, 0);
    temp3 = big_int_create(NULL, 0);
    big_int_mul_mod(temp1, curve_point.x, curve_point.x, curve.q); // temp1 = x^2
    big_int_mul_mod(temp2, curve_point.y, curve_point.y, curve.q); // temp2 = y^2
    big_int_mul_mod(temp3, temp1, temp2, curve.q); // temp3 = x^2 * y^2
    big_int_add_mod(temp1, temp1, temp2, curve.q); // temp1 = x^2 + y^2
    big_int_mul_mod(temp3, curve.d, temp3, curve.q); // temp3 = d * x^2 * y^2
    big_int_add_mod(temp3, big_int_one, temp3, curve.q); // temp3 = 1 + d * x^2 * x^2
    ck_assert_int_eq(big_int_compare(temp1, temp3), 0);

    // Same test with t = 1
    t = big_int_create(t, 1);
    curve_point = elligator_1_string_to_point(t, curve);

    temp1 = big_int_create(NULL, 0);
    temp2 = big_int_create(NULL, 0);
    temp3 = big_int_create(NULL, 0);
    big_int_mul_mod(temp1, curve_point.x, curve_point.x, curve.q); // temp1 = x^2
    big_int_mul_mod(temp2, curve_point.y, curve_point.y, curve.q); // temp2 = y^2
    big_int_mul_mod(temp3, temp1, temp2, curve.q); // temp3 = x^2 * y^2
    big_int_add_mod(temp1, temp1, temp2, curve.q); // temp1 = x^2 + y^2
    big_int_mul_mod(temp3, curve.d, temp3, curve.q); // temp3 = d * x^2 * y^2
    big_int_add_mod(temp3, big_int_one, temp3, curve.q); // temp3 = 1 + d * x^2 * x^2
    ck_assert_int_eq(big_int_compare(temp1, temp3), 0);

    // u * v * X * Y * x * (y + 1) != 0 (requires intermediate results)
    // Y^2 = X^5 + (r^2 - 2) * X^3 + X (requires intermediate results)

    big_int_destroy(t);
    big_int_destroy(temp1);
    big_int_destroy(temp2);
    big_int_destroy(temp3);

    free_curve(&curve);
    free_curve_point(&curve_point);
}
END_TEST


Suite *elligator_suite(void)
{
    Suite *s;
    TCase *tc_basic, *tc_advanced;

    s = suite_create("Elligator Test Suite");

    tc_basic = tcase_create("Basic Tests");

    tcase_add_test(tc_basic, test_curve1174);
    tcase_add_test(tc_basic, test_e2e);
    tcase_add_test(tc_basic, test_edge_cases);

    tc_advanced = tcase_create("Advanced Tests");
    tcase_add_test(tc_advanced, test_advanced_curve1174);
    tcase_add_test(tc_advanced, test_advanced_string_to_point);

    suite_add_tcase(s, tc_basic);
    suite_add_tcase(s, tc_advanced);

    return s;
}

 int main(void)
 {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = elligator_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }

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

// Macros
#if VERSION == 1

#define TEST_REF(x) (x)
#define TEST_BIG_INT_DEFINE(a) BigInt *a = big_int_create_from_chunk(NULL, 0, 0)
#define TEST_BIG_INT_DESTROY(a) big_int_destroy(a)

#define TEST_FREE_CURVE(a) free_curve(a)
#define TEST_FREE_CURVE_POINT(a) free_curve_point(a)

#else

#define TEST_REF(x) (&(x))
#define TEST_BIG_INT_DEFINE(a) BIG_INT_DEFINE_FROM_CHUNK(a, 0, 0)
#define TEST_BIG_INT_DESTROY(a) ((void) 0)
#define TEST_FREE_CURVE(a) ((void) 0)
#define TEST_FREE_CURVE_POINT(a) ((void) 0)

#endif

#define TEST_CURVE_POINT_CMP(curve, exp_x, exp_y)                              \
    do {                                                                       \
        ck_assert_int_eq(big_int_compare(TEST_REF((curve).x), (exp_x)), 0);    \
        ck_assert_int_eq(big_int_compare(TEST_REF((curve).y), (exp_y)), 0);    \
    } while (0)

/**
* \brief Test mapping a number to the curve and back
*/
START_TEST(test_curve1174)
{
    Curve curve;
    TEST_BIG_INT_DEFINE(c);
    TEST_BIG_INT_DEFINE(d);
    TEST_BIG_INT_DEFINE(r);

    init_curve1174(&curve);

    big_int_create_from_hex(c,
        "4D1A3398ED42CEEB451D20824CA9CB49B69EF546BD7E6546AEF19AF1F9E49E1");
    big_int_create_from_hex(d,
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB61");
    big_int_create_from_hex(r,
        "6006FBDA7649C433816B286006FBDA7649C433816B286006FBDA7649C43383");
    ck_assert_int_eq(big_int_compare(TEST_REF(curve.c), c), 0);
    ck_assert_int_eq(big_int_compare(TEST_REF(curve.d), d), 0);
    ck_assert_int_eq(big_int_compare(TEST_REF(curve.r), r), 0);

    TEST_BIG_INT_DESTROY(c);
    TEST_BIG_INT_DESTROY(d);
    TEST_BIG_INT_DESTROY(r);

    TEST_FREE_CURVE(&curve);
}
END_TEST

/**
* \brief Test mapping a number to the curve and back
*/
START_TEST(test_e2e)
{
    Curve curve;
    CurvePoint curve_point;

    TEST_BIG_INT_DEFINE(r);
    TEST_BIG_INT_DEFINE(t);
    TEST_BIG_INT_DEFINE(x);
    TEST_BIG_INT_DEFINE(y);

    init_curve1174(&curve);

    big_int_create_from_chunk(t, 7, 0);
    big_int_create_from_hex(x,
        "AB65983CF55A18C0E2C8BB8A156E030566D23767D6C1473ACFCF4D17439AC7");
    big_int_create_from_hex(y,
        "49C01F8D8C86ECB362B3952FA93ABD8CF512B09225BCEE9E76BC5E0C9A6E17E");

    // Map BigInt to curve point
    curve_point = elligator_1_string_to_point(t, curve);
    TEST_CURVE_POINT_CMP(curve_point, x, y);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    // Do the same with more complex inputs
    big_int_create_from_hex(t, "75BCD15");
    big_int_create_from_hex(x,
        "1A47D90E9F9C19AB846C9E100317F693607A1D16851FD05B40E7DA6FFF5BCF");
    big_int_create_from_hex(y,
        "5BBE4619CEA4729F94082B429693AC4B565F94CDA5D6D875689DE765C19A461");

    curve_point = elligator_1_string_to_point(t, curve);
    TEST_CURVE_POINT_CMP(curve_point, x, y);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    big_int_create_from_hex(t, "ABCDEF1234567899987654321ABCABCDEFDEF");
    big_int_create_from_hex(x,
        "7D4DD83C5E1F4E2DBC79CA0941102DE6376604C9D8A75EB41B6875010211B50");
    big_int_create_from_hex(y,
        "61C507D563235BEA439C1D53BDDF33E0D87C0041D3228CD0CA00C3D2A59025E");

    curve_point = elligator_1_string_to_point(t, curve);
    TEST_CURVE_POINT_CMP(curve_point, x, y);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    big_int_create_from_chunk(t, 2, 0);
    big_int_create_from_hex(x,
        "6F5374156B145FF8BB3288E0418F513B5D7BBBAB6E252EA1BC2DB6428E1454E");
    big_int_create_from_hex(y,
        "ED7F6014F111318ED7F6014F111318ED7F6014F111318ED7F6014F111318EC");

    curve_point = elligator_1_string_to_point(t, curve);
    TEST_CURVE_POINT_CMP(curve_point, x, y);

    // Map curve point back to BigInt
    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    // Negative t is not valid input for Elligator 1

    TEST_BIG_INT_DESTROY(t);
    TEST_BIG_INT_DESTROY(x);
    TEST_BIG_INT_DESTROY(y);
    TEST_BIG_INT_DESTROY(r);

    TEST_FREE_CURVE_POINT(&curve_point);
    TEST_FREE_CURVE(&curve);
}
END_TEST

/**
* \brief Test edges cases for Elligator mapping
*/
START_TEST(test_edge_cases)
{
    Curve curve;
    CurvePoint curve_point;

    TEST_BIG_INT_DEFINE(r);
    TEST_BIG_INT_DEFINE(t);
    TEST_BIG_INT_DEFINE(x);
    TEST_BIG_INT_DEFINE(y);

    // t = 1
    init_curve1174(&curve);

    big_int_create_from_chunk(t, 1, 0);
    big_int_create_from_chunk(x, 0, 0);
    big_int_create_from_chunk(y, 1, 0);

    curve_point = elligator_1_string_to_point(t, curve);
    TEST_CURVE_POINT_CMP(curve_point, x, y);

    r = elligator_1_point_to_string(curve_point, curve);
    ck_assert_int_eq(big_int_compare(r, t), 0);

    // t = -1 is not a valid input for the Elligator mapping

    TEST_BIG_INT_DESTROY(t);
    TEST_BIG_INT_DESTROY(x);
    TEST_BIG_INT_DESTROY(y);
    TEST_BIG_INT_DESTROY(r);

    TEST_FREE_CURVE_POINT(&curve_point);
    TEST_FREE_CURVE(&curve);
}
END_TEST

/**
* \brief Tests based on statements about curve1174 in paper
*/
START_TEST(test_advanced_curve1174)
{
    Curve curve;

    TEST_BIG_INT_DEFINE(q_mod4);
    TEST_BIG_INT_DEFINE(q_mod4_exp);
    TEST_BIG_INT_DEFINE(four);
    TEST_BIG_INT_DEFINE(temp1);
    TEST_BIG_INT_DEFINE(temp2);

    init_curve1174(&curve);

    big_int_create_from_chunk(q_mod4_exp, 3, 0);
    big_int_create_from_chunk(q_mod4, 0, 0);
    big_int_create_from_chunk(four, 4, 0);
    big_int_mod(q_mod4, TEST_REF(curve.q), four);

    ck_assert_int_eq(big_int_compare(q_mod4, q_mod4_exp), 0);

    /* c * (c - 1) * (c + 1) != 0
     * Actually calculating this with BigInts requires too many bits,
     * so I will just check that each part != 0 instead.
     */
    big_int_create_from_chunk(temp1, 0, 0);
    big_int_create_from_chunk(temp2, 0, 0);
    ck_assert_int_ne(big_int_compare(TEST_REF(curve.c), big_int_zero), 0);

    big_int_sub(temp1, TEST_REF(curve.c), big_int_one); // temp1 = curve.c - 1
    ck_assert_int_ne(big_int_compare(temp1, big_int_zero), 0);

    big_int_add(temp2, TEST_REF(curve.c), big_int_one); // temp2 = curve.c + 1
    ck_assert_int_ne(big_int_compare(temp2, big_int_zero), 0);

    /*
     * Check that d = -1174 mod q
     */
    big_int_create_from_chunk(temp1, 1174, 1);
    big_int_mod(temp1, temp1, TEST_REF(curve.q));
    ck_assert_int_eq(big_int_compare(temp1, TEST_REF(curve.d)), 0);

    // Check that d = -(c + 1)^2 / (c - 1)^2 mod q
    big_int_copy(temp1, TEST_REF(curve.c));
    big_int_add(temp1, temp1, big_int_one);
    big_int_mul_mod(temp1, temp1, temp1, TEST_REF(curve.q));
    big_int_neg(temp1, temp1); // temp1 = -(c + 1)^2

    big_int_copy(temp2, TEST_REF(curve.c));
    big_int_sub(temp2, temp2, big_int_one);

    big_int_mul_mod(temp2, temp2, temp2, TEST_REF(curve.q)); // temp1 = (c - 1)^2
    big_int_div_mod(temp1, temp1, temp2, TEST_REF(curve.q)); // temp1 = -(c + 1)^2 / (c - 1)^2 mod q
    ck_assert_int_eq(big_int_compare(temp1, TEST_REF(curve.d)), 0);

    // r != 0
    ck_assert_int_ne(big_int_compare(TEST_REF(curve.r), big_int_zero), 0);

    // d is not square
    big_int_chi(temp1, TEST_REF(curve.d), TEST_REF(curve.q));
    ck_assert_int_eq(big_int_compare(temp1, big_int_min_one), 0);

    TEST_BIG_INT_DESTROY(q_mod4_exp);
    TEST_BIG_INT_DESTROY(q_mod4);
    TEST_BIG_INT_DESTROY(four);
    TEST_BIG_INT_DESTROY(temp1);
    TEST_BIG_INT_DESTROY(temp2);

    TEST_FREE_CURVE(&curve);
}
END_TEST

/**
* \brief Tests based on statements about mapping in paper
*/
START_TEST(test_advanced_string_to_point)
{
    Curve curve;
    CurvePoint curve_point;

    TEST_BIG_INT_DEFINE(t);
    TEST_BIG_INT_DEFINE(temp1);
    TEST_BIG_INT_DEFINE(temp2);
    TEST_BIG_INT_DEFINE(temp3);

    init_curve1174(&curve);

    big_int_create_from_hex(t, "ABCDEF1234567899987654321ABCABCDEFDEF");

    curve_point = elligator_1_string_to_point(t, curve);

    // x^2 + y^2 = 1 + d * x^2 * y^2 (mod q)
    big_int_create_from_chunk(temp1, 0, 0);
    big_int_create_from_chunk(temp2, 0, 0);
    big_int_create_from_chunk(temp3, 0, 0);
    big_int_mul_mod(temp1, TEST_REF(curve_point.x), TEST_REF(curve_point.x),
        TEST_REF(curve.q)); // temp1 = x^2
    big_int_mul_mod(temp2, TEST_REF(curve_point.y), TEST_REF(curve_point.y),
        TEST_REF(curve.q)); // temp2 = y^2
    big_int_mul_mod(temp3, temp1, temp2, TEST_REF(curve.q)); // temp3 = x^2 * y^2
    big_int_add_mod(temp1, temp1, temp2, TEST_REF(curve.q)); // temp1 = x^2 + y^2
    big_int_mul_mod(temp3, TEST_REF(curve.d), temp3,
        TEST_REF(curve.q)); // temp3 = d * x^2 * y^2
    big_int_add_mod(temp3, big_int_one, temp3,
        TEST_REF(curve.q)); // temp3 = 1 + d * x^2 * x^2

    ck_assert_int_eq(big_int_compare(temp1, temp3), 0);

    // Same test with t = 1
    big_int_create_from_chunk(t, 1, 0);

    curve_point = elligator_1_string_to_point(t, curve);

    big_int_create_from_chunk(temp1, 0, 0);
    big_int_create_from_chunk(temp2, 0, 0);
    big_int_create_from_chunk(temp3, 0, 0);
    big_int_mul_mod(temp1, TEST_REF(curve_point.x), TEST_REF(curve_point.x),
        TEST_REF(curve.q)); // temp1 = x^2
    big_int_mul_mod(temp2, TEST_REF(curve_point.y), TEST_REF(curve_point.y),
        TEST_REF(curve.q)); // temp2 = y^2
    big_int_mul_mod(temp3, temp1, temp2, TEST_REF(curve.q)); // temp3 = x^2 * y^2
    big_int_add_mod(temp1, temp1, temp2, TEST_REF(curve.q)); // temp1 = x^2 + y^2
    big_int_mul_mod(temp3, TEST_REF(curve.d), temp3, TEST_REF(curve.q)); // temp3 = d * x^2 * y^2
    big_int_add_mod(temp3, big_int_one, temp3, TEST_REF(curve.q)); // temp3 = 1 + d * x^2 * x^2

    ck_assert_int_eq(big_int_compare(temp1, temp3), 0);

    // u * v * X * Y * x * (y + 1) != 0 (requires intermediate results)
    // Y^2 = X^5 + (r^2 - 2) * X^3 + X (requires intermediate results)

    TEST_BIG_INT_DESTROY(t);
    TEST_BIG_INT_DESTROY(temp1);
    TEST_BIG_INT_DESTROY(temp2);
    TEST_BIG_INT_DESTROY(temp3);

    TEST_FREE_CURVE(&curve);
    TEST_FREE_CURVE_POINT(&curve_point);
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

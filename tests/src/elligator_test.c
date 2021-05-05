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

    // TODO: Do the same with more complex inputs

    big_int_destroy(t);
    big_int_destroy(x);
    big_int_destroy(y);
    big_int_destroy(r);

    free_curve_point(&curve_point);
    free_curve(&curve);
}
END_TEST

// TODO: Check mapping edge cases, e.g. t = 1, -1 (cf. paper)
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

        // t = -1
        init_curve1174(&curve);
        t = big_int_create(NULL, -1);
        x = big_int_create(NULL, 0);
        y = big_int_create(NULL, 1);

        //curve_point = elligator_1_string_to_point(t, curve);
                // Fatal: Non-invertible number given as argument to big_int_inv!
        //ck_assert_int_eq(big_int_compare(curve_point.x, x), 0);
        //ck_assert_int_eq(big_int_compare(curve_point.y, y), 0);

        //r = elligator_1_point_to_string(curve_point, curve);
        //ck_assert_int_eq(big_int_compare(r, t), 0);

        big_int_destroy(t);
        big_int_destroy(x);
        big_int_destroy(y);
        big_int_destroy(r);

        free_curve_point(&curve_point);
        free_curve(&curve);
}
END_TEST

// TODO: Check statements in paper, most of them are included in elligator.sage
//       as assertions. Not sure we want to do this for intermediate results too
//       (that would require copy-pasting part of the implementation into the
//       test case) but we can for sure test e2e properties. E.g., that points
//       are actually on the curve or that curve params satisfy those equations.


Suite *elligator_suite(void)
{
    Suite *s;
    TCase *tc_basic;

    s = suite_create("Elligator Test Suite");

    tc_basic = tcase_create("Basic Tests");

    tcase_add_test(tc_basic, test_curve1174);
    tcase_add_test(tc_basic, test_e2e);
    tcase_add_test(tc_basic, test_edge_cases);

    suite_add_tcase(s, tc_basic);

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

/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This is a unit test file to test the "example" executable.
 * This file should serve as a template for the real unit tests.
 */

/*
 * Includes
 */
#include <check.h>
#include <stdlib.h>

// Include header files
#include "example.h"

/**
* Example test case checking the return value of get_local_macro
*/
START_TEST(test_get_local_macro)
{
    ck_assert_int_eq(get_local_macro(), 1);
}
END_TEST

/**
* Example test case checking the returned string of get_global_str
*/
START_TEST(test_get_global_str)
{
    ck_assert_str_eq(get_global_str(), "global test");
}
END_TEST

Suite *example_suite(void)
{
    Suite *s;
    TCase *tc_basic;

    s = suite_create("Example");

    tc_basic = tcase_create("Basic");

    tcase_add_test(tc_basic, test_get_local_macro);
    tcase_add_test(tc_basic, test_get_global_str);

    suite_add_tcase(s, tc_basic);

    return s;
}

 int main(void)
 {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = example_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }

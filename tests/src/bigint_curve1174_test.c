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
#include "bigint_curve1174.h"
#include "bigint.h"
#include "debug.h"

/**
* \brief Test modulo operation on BigInts
*/
START_TEST(test_modulo_operation)
{
    BIG_INT_DEFINE_PTR(a);
    BIG_INT_DEFINE_PTR(r);

    // q < a < 2^256
    big_int_create_from_hex(a,
        "CF8E255C938ED477789723C31E7376618974944FD1A3DBD0394BCA5818A16E9D");
    big_int_create_from_hex(r,
        "78E255C938ED477789723C31E7376618974944FD1A3DBD0394BCA5818A16F7E");

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a >= 2^256
    big_int_create_from_hex(a,
        "3184515284CEE5EFAFBDC78D09C71F3B28F498A66EFD1E81E20FE5BA09528FA4" \
        "EDDC48FAF407C8589A90161F23290C55B302AFD11728203E96FB7519CEC15DB1");
    big_int_create_from_hex(r,
        "2B7C5D05CCA75FE501094CA232C2EE3C62E6B0DF3EA725CE8DDE6644BA335E5");

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Negative a < a < 2^256
    big_int_create_from_hex(a,
        "-CF8E255C938ED477789723C31E7376618974944FD1A3DBD0394BCA5818A16E9D");
    big_int_create_from_hex(r,
        "71DAA36C712B888768DC3CE18C899E768B6BB02E5C242FC6B435A7E75E9079");

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Negative a, a >= 2^256
    big_int_create_from_hex(a,
        "-3184515284CEE5EFAFBDC78D09C71F3B28F498A66EFD1E81E20FE5BA09528FA4" \
         "EDDC48FAF407C8589A90161F23290C55B302AFD11728203E96FB7519CEC15DB1");
    big_int_create_from_hex(r,
        "5483A2FA3358A01AFEF6B35DCD3D11C39D194F20C158DA31722199BB45CCA12");

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a = 0
    big_int_create_from_chunk(a, 0, 0);
    big_int_create_from_chunk(r, 0, 0);

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a < q
    big_int_create_from_hex(a,
        "74118038E8C893F1033F7C44CEC2B2E7636A2C7275929117FCB8F51459922B9");
    big_int_create_from_hex(r,
        "74118038E8C893F1033F7C44CEC2B2E7636A2C7275929117FCB8F51459922B9");

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // a < q; negative a
    big_int_create_from_hex(a,
        "-74118038E8C893F1033F7C44CEC2B2E7636A2C7275929117FCB8F51459922B9");
    big_int_create_from_hex(r,
        "BEE7FC717376C0EFCC083BB313D4D189C95D38D8A6D6EE803470AEBA66DD3E");

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // q | a
    big_int_create_from_hex(a,
        "9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4C"); // a = 20 * q
    big_int_create_from_chunk(r, 0, 0);

    big_int_curve1174_mod(a, a);
    ck_assert_int_eq(big_int_compare(a, r), 0);
}
END_TEST

/**
* \brief Test addition of BigInts modulo q
*/
START_TEST(test_add_mod)
{
    BIG_INT_DEFINE_PTR(a);
    BIG_INT_DEFINE_PTR(b);
    BIG_INT_DEFINE_PTR(r);

    // Basic test
    big_int_create_from_hex(a,
        "3C76710CD95B84858D9E9390EC00EA32782F8D7436A812F919C5E2987F451FE");
    big_int_create_from_hex(b,
        "77F85143C07FAB7B1AAA3213A466DD2127583A01684EBADE1219E5A1602BB78");
    big_int_create_from_hex(r,
        "346EC25099DB3000A848C5A49067C7539F87C7759EF6CDD72BDFC839DF70D7F");

    big_int_curve1174_add_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Mixed signs
    big_int_create_from_hex(a,
        "-3C76710CD95B84858D9E9390EC00EA32782F8D7436A812F919C5E2987F451FE");
    big_int_create_from_hex(b,
        "77F85143C07FAB7B1AAA3213A466DD2127583A01684EBADE1219E5A1602BB78");
    big_int_create_from_hex(r,
        "3B81E036E72426F58D0B9E82B865F2EEAF28AC8D31A6A7E4F8540308E0E697A");

    big_int_curve1174_add_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a,
        "3C76710CD95B84858D9E9390EC00EA32782F8D7436A812F919C5E2987F451FE");
    big_int_create_from_hex(b,
        "-77F85143C07FAB7B1AAA3213A466DD2127583A01684EBADE1219E5A1602BB78");
    big_int_create_from_hex(r,
        "447E1FC918DBD90A72F4617D479A0D1150D75372CE59581B07ABFCF71F1967D");

    big_int_curve1174_add_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a,
        "-3C76710CD95B84858D9E9390EC00EA32782F8D7436A812F919C5E2987F451FE");
    big_int_create_from_hex(b,
        "-77F85143C07FAB7B1AAA3213A466DD2127583A01684EBADE1219E5A1602BB78");
    big_int_create_from_hex(r,
        "4B913DAF6624CFFF57B73A5B6F9838AC6078388A61093228D42037C6208F278");

    big_int_curve1174_add_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);
}
END_TEST

/**
* \brief Test subtraction of BigInts modulo q
*/
START_TEST(test_sub_mod)
{
    BIG_INT_DEFINE_PTR(a);
    BIG_INT_DEFINE_PTR(b);
    BIG_INT_DEFINE_PTR(r);

    // Basic test
    big_int_create_from_hex(a,
        "195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    big_int_create_from_hex(b,
        "5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    big_int_create_from_hex(r,
        "3E1F9BCB2FB90B4601D5A0ACE0FD46A9550D5CD9B2AEB5345D7F19C84F4C4A3");

    big_int_curve1174_sub_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Mixed signs
    big_int_create_from_hex(a,
        "-195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    big_int_create_from_hex(b,
        "5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    big_int_create_from_hex(r,
        "B6789569B16080DF054C3FD5BF76C9893C7508DF62A0BB19665222877E6789");

    big_int_curve1174_sub_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a,
        "195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    big_int_create_from_hex(b,
        "-5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    big_int_create_from_hex(r,
        "749876A964E9F7F20FAB3C02A40893676C38AF7209D5F44E699ADDD7881986E");

    big_int_curve1174_sub_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    big_int_create_from_hex(a,
        "-195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    big_int_create_from_hex(b,
        "-5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    big_int_create_from_hex(r,
        "B6789569B16080DF054C3FD5BF76C9893C7508DF62A0BB19665222877E6789");

    big_int_curve1174_sub_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);
}
END_TEST

/**
* \brief Test multiplication of BigInts modulo q
*/
START_TEST(test_mul_mod)
{
    BIG_INT_DEFINE_PTR(a);
    BIG_INT_DEFINE_PTR(b);
    BIG_INT_DEFINE_PTR(r);

    // Basic test
    big_int_create_from_hex(a, "ABCD4569AB3096134DD");
    big_int_create_from_hex(b, "56AA098765");
    big_int_create_from_hex(r, "3A291206AC264ECED165893FF6631");

    big_int_curve1174_mul_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Test mod operation with intermediate result that is larger than 256 bits
    a = big_int_create_from_hex(a,
        "195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    b = big_int_create_from_hex(b,
        "5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    r = big_int_create_from_hex(r,
        "49D6974B07A3EC152F17380C6C4AD33F6D97BB72EE4771F4BFB7A50338B96CF");

    big_int_curve1174_mul_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Trigger the rare case (D6 in Knuth's Book)
    a = big_int_create_from_hex(a,
            "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    b = big_int_create_from_hex(b,
            "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");
    r = big_int_create_from_hex(r,
            "200000000000000000000000000000000000000000000000000000000000012");

    big_int_curve1174_mul_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    // Mixed signs
    a = big_int_create_from_hex(a,
        "-195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    b = big_int_create_from_hex(b,
        "5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    r = big_int_create_from_hex(r,
        "362968B4F85C13EAD0E8C7F393B52CC09268448D11B88E0B40485AFCC746928");

    big_int_curve1174_mul_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    a = big_int_create_from_hex(a,
        "195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    b = big_int_create_from_hex(b,
        "-5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    r = big_int_create_from_hex(r,
        "362968B4F85C13EAD0E8C7F393B52CC09268448D11B88E0B40485AFCC746928");

    big_int_curve1174_mul_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);

    a = big_int_create_from_hex(a,
        "-195C093A4A51819C08C06E57C282ED0860A30625DE4254C1638CFBCFEBB2E8D");
    b = big_int_create_from_hex(b,
        "-5B3C6D6F1A98765606EACDAAE185A65F0B95A94C2B939F8D060DE2079C669E1");
    r = big_int_create_from_hex(r,
        "49D6974B07A3EC152F17380C6C4AD33F6D97BB72EE4771F4BFB7A50338B96CF");

    big_int_curve1174_mul_mod(a, a, b);
    ck_assert_int_eq(big_int_compare(a, r), 0);
}
END_TEST

/**
* \brief Test division of BigInts modulo q
*/
START_TEST(test_div_mod)
{
    BIG_INT_DEFINE_PTR(a);
    BIG_INT_DEFINE_PTR(b);
    BIG_INT_DEFINE_PTR(r);
    BIG_INT_DEFINE_PTR(x);

    big_int_create_from_hex(a, "ABCD4569AB3096134DD");
    big_int_create_from_hex(b, "56AA098765");
    big_int_create_from_hex(r,
        "2BC89F199D405D9F5BFBAE9D39DE432CBE2CBA178C8338CE36DB15230F1983E");

    big_int_curve1174_div_mod(x, a, b);
    ck_assert_int_eq(big_int_compare(x, r), 0);

    big_int_create_from_hex(a,
        "33DF8D4DC234509B75C12BD784249BC190D0F7D4D2DB842A54BB0311053F52");
    big_int_create_from_hex(b,
        "423357D8C4B27F8A9703050F84629B0700563A9C33855AA53C134EE714060E");
    big_int_create_from_hex(r,
        "3AA858FE97FE64871464CA7B7163573E7777F45E8DEBA44E50E68CB5FE3B64E");

    big_int_curve1174_div_mod(x, a, b);
    ck_assert_int_eq(big_int_compare(x, r), 0);
}
END_TEST

/**
* \brief Test finding the modular inverse of BigInts
*/
START_TEST(test_modulo_inverse)
{
    BIG_INT_DEFINE_PTR(a);
    BIG_INT_DEFINE_PTR(ainv_exp);

    big_int_create_from_hex(a, "76101CAD986E75478DAAD160");
    big_int_create_from_hex(ainv_exp,
        "6E590E98D1F28843F19A108DF2153AEC5901C39F34D68A1FE43C08B8F2B75DE");

    BIG_INT_CURVE1174_INV(a, a);
    ck_assert_int_eq(big_int_compare(a, ainv_exp), 0);

    // Negative a
    big_int_create_from_hex(a, "-76101CAD986E75478DAAD160");
    big_int_create_from_hex(ainv_exp,
        "11A6F1672E0D77BC0E65EF720DEAC513A6FE3C60CB2975E01BC3F7470D48A19");

    BIG_INT_CURVE1174_INV(a, a);
    ck_assert_int_eq(big_int_compare(a, ainv_exp), 0);

    // a = 7
    big_int_create_from_chunk(a, 7, 1);
    big_int_create_from_hex(ainv_exp,
        "49249249249249249249249249249249249249249249249249249249249248D");

    BIG_INT_CURVE1174_INV(a, a);
    ck_assert_int_eq(big_int_compare(a, ainv_exp), 0);
}
END_TEST

/**
* \brief Test raising BigInts to a (BigInt) power
*/
START_TEST(test_power)
{
    BIG_INT_DEFINE_PTR(b);
    BIG_INT_DEFINE_PTR(e);
    BIG_INT_DEFINE_PTR(r);

    big_int_create_from_hex(b, "4F2B8718");
    big_int_create_from_chunk(e, 23, 0);
    big_int_create_from_hex(r,
        "29CF20A15DB7E64D6A773386D6392402101CAB5BEF9A73BDFD2141DD9745AE2");

    big_int_curve1174_pow(b, b, e);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Base close to max size & trigger rare case in div_rem
    big_int_create_from_hex(b,
        "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD");
    big_int_create_from_chunk(e, 5, 0);
    big_int_create_from_hex(r,
        "7C9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");

    big_int_curve1174_pow(b, b, e);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Test exponent 0
    big_int_create_from_hex(b, "67AFE4589B");
    big_int_create_from_chunk(e, 0, 0);
    big_int_create_from_chunk(r, 1, 0);

    big_int_curve1174_pow(b, b, e);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Test base 1
    big_int_create_from_chunk(b, 1, 0);
    big_int_create_from_hex(e, "67AFE4589BABCD");
    big_int_create_from_chunk(r, 1, 0);

    big_int_curve1174_pow(b, b, e);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Odd power
    big_int_create_from_hex(b, "ABCDEF123456789");
    big_int_create_from_hex(e, "89BABC1");
    big_int_create_from_hex(r,
        "76E90A95365C24F36ACB354316A91479DC40A224B60B909F81CC9BEC98DBA90");

    big_int_curve1174_pow(b, b, e);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Even power
    big_int_create_from_hex(b, "ABCDEF123456789");
    big_int_create_from_hex(e, "89BABC0");
    big_int_create_from_hex(r,
        "6490D135FEA9435C9C06E293824B2C11EC445D5A3387EBE1A70C929E411313E");

    big_int_curve1174_pow(b, b, e);
    ck_assert_int_eq(big_int_compare(b, r), 0);
}
END_TEST


/**
* \brief Test raising BigInts to a small (< 2^64) power
*/
START_TEST(test_power_small)
{
    BIG_INT_DEFINE_PTR(b);
    BIG_INT_DEFINE_PTR(r);

    big_int_create_from_hex(b, "4F2B8718");
    big_int_create_from_hex(r,
        "29CF20A15DB7E64D6A773386D6392402101CAB5BEF9A73BDFD2141DD9745AE2");

    big_int_curve1174_pow_small(b, b, 23);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Base close to max size & trigger rare case in div_rem
    big_int_create_from_hex(b,
        "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD");
    big_int_create_from_hex(r,
        "7C9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7");

    big_int_curve1174_pow_small(b, b, 5);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Test exponent 0
    big_int_create_from_hex(b, "67AFE4589B");
    big_int_create_from_chunk(r, 1, 0);

    big_int_curve1174_pow_small(b, b, 0);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Odd power
    big_int_create_from_hex(b, "ABCDEF123456789");
    big_int_create_from_hex(r,
        "76E90A95365C24F36ACB354316A91479DC40A224B60B909F81CC9BEC98DBA90");

    big_int_curve1174_pow_small(b, b, 0x89BABC1);
    ck_assert_int_eq(big_int_compare(b, r), 0);

    // Even power
    big_int_create_from_hex(b, "ABCDEF123456789");
    big_int_create_from_hex(r,
        "6490D135FEA9435C9C06E293824B2C11EC445D5A3387EBE1A70C929E411313E");

    big_int_curve1174_pow_small(b, b, 0x89BABC0);
    ck_assert_int_eq(big_int_compare(b, r), 0);
}
END_TEST

/**
* \brief Test chi function on BigInts
*/
START_TEST(test_chi)
{
    BIG_INT_DEFINE_PTR(r);
    BIG_INT_DEFINE_PTR(t);
    BIG_INT_DEFINE_PTR(s);
    BIG_INT_DEFINE_PTR(u);
    BIG_INT_DEFINE_PTR(v);

    // Tests on bigger numbers: q = 2^61 - 1
    big_int_create_from_chunk(t, 0, 0);
    big_int_create_from_chunk(r, 0, 0);

    big_int_curve1174_chi(t, t);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // Is square
    big_int_create_from_hex(t,
        "64548488BB3F9FBD9A0A0878BD075651BCA8692167A6D40095CBF9EC4465CC8");
    big_int_create_from_chunk(r, 1, 0);

    big_int_curve1174_chi(t, t);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    // Special case 1 (always square)
    big_int_create_from_chunk(t, 1, 1);
    big_int_create_from_chunk(r, 1, 1);

    big_int_curve1174_chi(t, t);
    ck_assert_int_eq(big_int_compare(t, r), 0);

    /* Advanced tests (as specified in the paper).
     * Most of these are actually pretty trivial.
     */

    // chi(chi(t)) = chi(t)
    big_int_create_from_hex(t, "3626229738A3B8");

    big_int_curve1174_chi(t, t);
    big_int_curve1174_chi(s, t);
    ck_assert_int_eq(big_int_compare(t, s), 0);

    // 0 is not a square
    big_int_create_from_chunk(t, 0, 0);
    big_int_create_from_chunk(r, 0, 0);

    ck_assert_int_eq(big_int_compare(t, r), 0);

    // chi(st) = chi(s) * chi(t)
    // s and t both square
    big_int_create_from_hex(s,
        "64548488BB3F9FBD9A0A0878BD075651BCA8692167A6D40095CBF9EC4465CC8");
    big_int_create_from_hex(t,
        "242FBB0B3EDCBC352EF808A0BE30889985443B61BB5D2BDD2472741ED26D875");

    big_int_curve1174_mul_mod(r, s, t);
    big_int_curve1174_chi(r, r); // chi(st)

    big_int_curve1174_chi(u, s); // chi(s)
    big_int_curve1174_chi(v, t); // chi(t)
    big_int_curve1174_mul_mod(u, u, v); // chi(s) * chi(t);

    ck_assert_int_eq(big_int_compare(r, u), 0);

    // s square, t non-square
    big_int_create_from_hex(s,
        "64548488BB3F9FBD9A0A0878BD075651BCA8692167A6D40095CBF9EC4465CC8");
    big_int_create_from_hex(t,
        "6C4BE8460BE7FC0E1F92C249742356CF46817EB808E865689F8198183374CD6");

    big_int_curve1174_mul_mod(r, s, t);
    big_int_curve1174_chi(r, r); // chi(st)

    big_int_curve1174_chi(u, s); // chi(s)
    big_int_curve1174_chi(v, t); // chi(t)
    big_int_curve1174_mul_mod(u, u, v); // chi(s) * chi(t);

    ck_assert_int_eq(big_int_compare(r, u), 0);

    // chi(st) = chi(s) * chi(t) for s or t = 0 is trivial.

    // chi(1/t) = chi(t) = 1/chi(t) if t != 0
    // NOTE: I don't really understand the chi(1/t) part since chi is defined for integers only?
    big_int_create_from_hex(t, "3626229738A3B9");

    big_int_curve1174_chi(r, t); // chi(t)

    BIG_INT_CURVE1174_INV(s, r); // 1/chi(t)

    BIG_INT_CURVE1174_INV(t, t);
    big_int_curve1174_chi(t, t); // chi(1/t)

    ck_assert_int_eq(big_int_compare(r, s), 0);
    ck_assert_int_eq(big_int_compare(r, t), 0);
}
END_TEST


Suite *bigints_suite(void)
{
    Suite *s;
    TCase *tc_modular_arith, *tc_advanced_ops;

    s = suite_create("BigInt Curve1174 Test Suite");

    tc_modular_arith = tcase_create("Modular Arithmetic");
    tc_advanced_ops = tcase_create("Advanced Operations");

    tcase_add_test(tc_modular_arith, test_modulo_operation);
    tcase_add_test(tc_modular_arith, test_modulo_inverse);
    tcase_add_test(tc_modular_arith, test_add_mod);
    tcase_add_test(tc_modular_arith, test_sub_mod);
    tcase_add_test(tc_modular_arith, test_mul_mod);
    tcase_add_test(tc_modular_arith, test_div_mod);

    tcase_add_test(tc_advanced_ops, test_power);
    tcase_add_test(tc_advanced_ops, test_power_small);
    tcase_add_test(tc_advanced_ops, test_chi);

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

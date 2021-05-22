# Optimization Notes by Freya

## Multiplication

### Basic

#### Miro's suggestions:
* strength reduction
* avoiding unnecessary function calls
* remove error checking, state assumptions in function description!

#### Ideas from meeting:
* `% BIG_INT_RADIX` could be replaced by `&`

#### Freya ideas:
* Check with Miro: can we remove the final `copy`?
* Maybe I can remove the `/` in the innermost `for` loop.
* Are we accessing consecutively?
* Look into `big_int_prune`

#### Replace % with &
x mod powers of 2 can be calculated as x mod (power of 2) - 1. I created a constant
`BIGINT_RADIX_FOR_MOD = BIGINT_RADIX - 1` in `bigint.h` and used this to replace
the % with an & in the inner loop of the `big_int_mul` function in `bigint.c`.

####
At Miro's request I created a `big_int_mul_single_chunk` function in `big_int.c` which takes
a regular BigInt and a BigInt with only one chunk as parameters. Making the assumption that
the second parameter is only 1 chunk allowed me to remove the outer `for` loop. Based on
the quick benchmark I ran, this seems to be about 3 times faster than running the regular
`big_int_mul` function on random BigInts, but I didn't check to see how fast `big_int_mul`
is if you give it a single chunk input.

### AVX

#### Ideas from meeting:
* Vectorise to perform 4 mults at once

### Cache

## Elligator Point-to-String
#### Ideas from meeting:
* Chris pre-computed some curve values

#### Precompute curve values
Some of the values in the computation only depend on curve values and not on the curve point
itself. Since the curve we are using is always Curve1174, I added these values as fields in the 
`Curve` struct in `elligator.h` and assigned them pre-computed values in `init_curve1174` in
`elligator.c`.

I added the following precomputed values:
* `e (q + 1)/4`. This value is used to calculate `X`.
* `s_times_c_min_one = s(c - 1)`. This is one of the factors of the input to `chi` that doesn't
depend on the curve point.
* `one_over_c_squared = 1 / c^2`. This is part of another factor of the input to the `chi` function.
* `q_half = (q-1)/2`. This is to perform a check right at the end of `elligator_1_point_to_string`. 

#### Replace powers of 2 with multiplications
I tried replacing two calls to `big_int_pow` where the exponent was `big_int_two` with 
a call to `big_int_mul_mod` instead as an alternative way to square numbers, but this
seemed to make the benchmarks run slightly slower so I changed it back.



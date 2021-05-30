# Optimization Notes by Freya

## Multiplication

### Basic

#### Replace % with &
x mod powers of 2 can be calculated as x mod (power of 2) - 1. I created a constant
`BIGINT_RADIX_FOR_MOD = BIGINT_RADIX - 1` in `bigint.h` and used this to replace
the % with an & in the inner loop of the `big_int_mul` function in `bigint.c`.

#### big_int_mul_single_chunk
At Miro's request I created a `big_int_mul_single_chunk` function in `big_int.c` which takes
a regular BigInt and a BigInt with only one chunk as parameters. Making the assumption that
the second parameter is only 1 chunk allowed me to remove the outer `for` loop. Based on
the quick benchmark I ran, this seems to be about 3 times faster than running the regular
`big_int_mul` function on random BigInts, but I didn't check to see how fast `big_int_mul`
is if you give it a single chunk input.

#### Replace / with <<
I was able to replace the division in the innermost loop with a shift operation.
'carry /= BIGINT_RADIX;' became 'carry >>= BIGINT_CHUNK_BIT_SIZE;'.

#### No aliasing assumption
In the original big_int_mul function, a local variable `r_loc` is created and its final
value is copied into the output parameter `r` before returning. Since the multiplication
function is used frequently, this results in many copy operations during one execution of
the Elligator algorithm. The copy was necessary to prevent errors when aliasing is present
in the parameters, for example the common input `big_int_mul(a, a, b)` which computes
a = a + b. Here the output parameter `a` is the same as one of the input parameters.

I removed the local variable `r_loc` and the final copy to improve the efficient of `big_int_mul`,
but this required making an assumption that no aliasing is present in the `big_int_mul` parameters.
This required me to go back to the tests, benchmark code, big_int and Elligator implementation and refactor them to remove aliasing from all calls to `big_int_mult` and its related functions (e.g. `big_int_square`,
`big_int_mul_mod`, `big_int_curve1174_mul_mod`, etc.). The quickest fix to ensure that tests passed was
to replace the aliased output variable with a temporary local variable, and then copy the local
variable back into the desired output variable. For example:
```
big_int_mul(a, a, b);
```
becomes
```
big_int_mul(temp, a, b);
big_int_copy(a, temp);
```

While this solution was acceptable for tests where efficiency is not high priority, it was not ideal for
the `big_int` and Elligator functions we were trying to optimise. In some places I was able to refactor
the code to avoid doing the copy. In `big_int_pow` and related functions, for example, we solved the issue
by alternating between two variables to store results:
```
while ...
{
    big_int_mul(a, a, b) // a = a * b
    ...
    big_int_mul(a, a, c) // a = a * c
}
return a;
```
became
```
while ...
{
    big_int_mul(a1, a2, b); // a1 = a2 * b
    ...
    big_int_mul(a2, a1, b); // a2 = a1 * b
}
return a2;
```

### AVX

#### Ideas from meeting:
* Vectorise to perform 4 mults in parallel
* Assume most big_ints are 8 chunks. Then write a dedicated function to multiply
8-chunk big_ints with an unrolled loop. This should be easier to vectorise than the
standard multiplication function. In the standard multiplication function, add a check to
see if the inputs are 8-chunks. If so, call the special 8-chunk function. If not continue as normal.
* We could verify that most big_ints are 8-chunks by counting calls to the regular multiplication function
vs the 8-chunk version.

### Cache

## Elligator Point-to-String

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

Some of these optimisations were rendered obsolete by Miro's Curve1174-specific functions and so were
later removed.

#### Replace powers of 2 with squares
Calls to `big_int_pow` raising big_ints to the power of 2 were replaced with the `big_int_square` function
developed by Chris. 

#### Curve-specific functions
All calls to generic functions such as `big_int_mul_mod` were replaced with the Curve1174-specific functions developed by Miro, such as `big_int_curve1174_mul_mod`. 


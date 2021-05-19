# Optimization Notes by Miro

## Profiling
Intel VTune is not available on MacOS. Instead, Apple's Instruments (the binary is called `xcrun xctrace`) is the recommended way to measure performance.

## BigInt Refactoring
- Benchmark `memcpy` to see if the more dense 32-bit representation would be beneficial.

### Time Profiler
#### BENCH\_TYPE\_ADD\_RANDOM
Number: 12

#### BENCH\_TYPE\_SUB\_RANDOM
Number: 17

#### BENCH\_TYPE\_DIVREM\_RANDOM
Number: 23

#### BENCH\_TYPE\_CHI
Number: 38

## BigInt Refactoring
Optimizations:
- Remove `alloc_size` and code for potential dynamic allocation.
- Add a chunks array of fixed size to the BigInt struct.
- Remove `big_int_alloc` and `big_int_calloc`
- Remove any checks for `NULL` pointers in the result array. They always have to be allocated, since we don't allocate anything dynamically anymore.
    - Also remove other checks and write them down as assumptions
- Change `big_int_copy` to a single `memcpy`, which copies all metadata and chunks in one single go
- Change `big_int_create` to only assign values and not malloc
    - Replace all `big_int_create` with a `NULL` argument to take heap variable
- Change `big_int_create_from_dbl_chunk` to only assign values and not malloc
- Remove obsolete `big_int_destroy`
- Remove invertibility check from `big_int_inv` (`g = 1`)
- Avoid copying intermediate results in `big_int_egcd` by directly writing to `EgcdResult res`
- **We should probably make the simplifying assumption that no inputs alias.**
- Change `Curve`, `CurvePoint`, and `EgcdResult` to directly hold `BigInt` instead of only pointers
    - Remove corresponding obsolete `free_*` functions
- Initialize curve parameters in `init_curve1174` directly from hex strings instead of computing them whenever possible
- Change GCD to return pointer to `EgcdResult`, add `EgcdResult` as argument
- Refactor tests and benchmarking to new APIs

## Cheap Modulo Operation for Curve1174
Our curve uses `q = 2^251 - 9` as modulus. This means we have the following equation:
```
2^251 - 9 = 0 (mod q)
=> 2^251 = 9 (mod q)
=> 2^256 = 288 (mod q)
```

Thus, we can split a number `X` of 512 bits into two parts: `X1 * 2^256 + X0`, where `X1` are the higher 256 bits and `X0` are the lower 256 bits. Then, we can compute `mod q` as follows:
```
X = X1 * 2^256 + X0 = X1 * 288 + X0 (mod q)
```

This is a lot cheaper than using division to compute the modulus.

### Second Observation
For numbers `q < X < 2^256` we cannot use the above trick. However, we can use repeated substraction instead of division: since `2^256 / q < 33`, we have to do between `1` and `33` subtractions until `X < q`. Depending on how much we can optimize subtraction in comparison to `divrem`, this could be cheaper.

This could also be **parallelized**: we could precompute `a_i * q` for `a_i \in [1, 33]` and then compute `X - a_i` in parallel.
We could also do **binary search** for the correct `a_i`: the first one where the result is not negative.

## Function Inlining
```
TODO
```
Look into compiler attributes to force inlining.

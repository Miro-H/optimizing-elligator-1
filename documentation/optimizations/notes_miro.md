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
This turns out to not be a good idea: first, we do not need so many subtractions (see below). Second, they all operate on BigInts, so doing them in parallel is not that beneficial, since likely a single operation already keeps all execution units fairly busy.

We could also do **binary search** for the correct `a_i`: the first one where the result is not negative. In a group discussion, we realized that we do not need to perform subtractions in this search: we simply need up to six comparison operations to learn which `a_i * q` is the first one that is smaller than `X` and then subtract only that one!

I generated those precomputed `a_i * q` values with the following script:
```python
for a in range(1, 34):
    h = hex(a * q)[2:]
    chunks = []
    size_add = ""
    for i in range(len(h), 8, -8):
        chunks.append("0x" + h[i-8:i])
    if i > 8:
        chunks.append("0x" + h[:i-8])
        size_add = "+ 1"
    print(f"#define Q_{a}_CHUNK_0 {chunks[0]}")
    print(f"#define Q_{a}_CHUNK_7 {chunks[7]}")
    chunks[0] = f"Q_{a}_CHUNK_0"
    chunks[7] = f"Q_{a}_CHUNK_7"
    s = ", ".join(chunks)
    print(f"BIG_INT_DEFINE_STATIC_STRUCT_PTR(q_{a}, 0, 0, Q_CHUNKS{size_add}, ({s}));\n")

```

Intentionally no mod reduction in the 288 case for `mul`, since we do one later. We know that our intermediate values are never larger than `(q-1)^2` and `288 * (q-1)^2 < 2^512`. (See code).

## Modular Inverse
Since `q` is prime, there are alternatives to using the normal GCD algorithm for calculating the inverse:
- **Fermat's theorem:** for `0 < a < q`, we have `a^(q-1) = 1 (mod q)` by Fermat's theorem. Thus, the inverse is `a^-1 = a^(q-2) (mod q)`. Therefore, we can do `log(q)` multiplications instead of `log(q)` expensive divisions (in expectation).
- **Right-Shift Algorithm:** Based on work of Penk and using Montgommery inverses. Described [here](https://link.springer.com/content/pdf/10.1007%2F3-540-36400-5_6.pdf). Look also at [this](https://www.researchgate.net/publication/3387259_Improved_Montgomery_modular_inverse_algorithm) and [this](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=863048).

## Modular Exponentiation
Observations:
1. Many power operations are to the power of small integers. Thus, add a function that only takes exponents of at most 64 bits in size.
2. We don't need to shift the entire exponent `e`, we can just operate on it chunk by chunk! This enables us to use much cheaper integer shifts!
3. We have some special exponents (`(q-1)/2` and `(q+1)/4`) that have a lot of one bits set. We can add special exponentiation functions for them with way less branching (add more AVX potential!).

## Chi
Switch to use the special exponentiation function described above.

Also, I changed the return value to `int8_t` instead of a BigInt.

## Function Inlining
```
TODO
```
Look into compiler attributes to force inlining.

## Comparison Plot Curve1174 vs. Previous
Make a comparison plot between those functions as follows.


Create logs for the normal BigInt functions.

For this, look at the current `benchmark_types.h` file and find the numbers for the following benchmarks (here are the current numbers, they might have changed!):
- `BENCH_TYPE_ADD_MOD_CURVE`: 25
- `BENCH_TYPE_SUB_MOD_CURVE`: 28
- `BENCH_TYPE_MUL_MOD_CURVE`: 31
- `BENCH_TYPE_DIV_MOD_CURVE`: 34
- `BENCH_TYPE_MOD_CURVE`: 38
- `BENCH_TYPE_INV`: 40
- `BENCH_TYPE_POW_CURVE`: 41
- `BENCH_TYPE_COMPARE`: 45
- `BENCH_TYPE_CHI`: 47

Then create logs for those benchmarks:
```
BENCHMARKS="25 28 31 34 38 40 41 43 44 45 48 50" SETS=10 REPS=1000 make run-runtime-benchmark
```

Create logs for the Curve1174 functions:
```
SETS=10 REPS=1000 make run-runtime-benchmark-curve1174
```

Write down the log folders of the previous make runs and make the comparison plot:
```
LOGS_NAMES="general curve1174" LOGS_DIRS="<path/to/general/logs> <path/to/curve1174/logs>" SETS=10 REPS=1000 make plot-comparison
```

## Case Reduction
- For the `chi` function, we can remove the special case `t = 0`, since for Elligator we are guaranteed to give non-zero arguments to `chi`. This is a consequence of the lemma `XY =/= 0` in the proof of the map in section 3.2 of [the Elligator paper](https://elligator.cr.yp.to/elligator-20130828.pdf). Because all three occurences of `chi` in the map are in a product producing `Y`. If one of the would have input `0`, its output would be `0` and thus the whole product would lead to `Y = 0`.

## Redefine API
Redefine functions to perform inplace modifications where sensible (instead of allowing a different pointer for the result) and **assume no aliasing** for other functions.

For some power functions, I made the assumption that `r` is already initialized to one by the caller. This should be done as follows:
```
BIG_INT_DEFINE_FROM_CHUNK(r, 0, 1);
```
When `r` is defined. This saves us a call to `big_int_create_from_chunk`, since the macro actually defines `r` directly on initialization (which is certainly not slower, I'm pretty sure it's faster).

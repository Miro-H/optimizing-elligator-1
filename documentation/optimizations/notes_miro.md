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

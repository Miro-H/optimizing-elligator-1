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
- `BENCH_TYPE_ADD_MOD_CURVE`: 26
- `BENCH_TYPE_SUB_MOD_CURVE`: 29
- `BENCH_TYPE_MUL_MOD_CURVE`: 32
- `BENCH_TYPE_DIV_MOD_CURVE`: 35
- `BENCH_TYPE_MOD_CURVE`: 39
- `BENCH_TYPE_MOD_512_CURVE`: 40
- `BENCH_TYPE_INV`: 42
- `BENCH_TYPE_POW_CURVE`: 43
- `BENCH_TYPE_POW_SMALL_CURVE`: 45
- `BENCH_TYPE_POW_Q_M1_D2_CURVE`: 46
- `BENCH_TYPE_POW_Q_P1_D4_CURVE`: 47
- `BENCH_TYPE_COMPARE`: 50
- `BENCH_TYPE_CHI`: 52

Then create logs for those benchmarks:
```
BENCHMARKS="26 29 32 35 39 40 42 43 45 46 47 50 52" SETS=10 REPS=1000 make run-runtime-benchmark
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

## Compiler Flags
Generate comparison plots for different optimization flags as described below.

### Script
Just execute
```bash
VERSION=2 SETS=10 REPS=1000 ./scripts/make_opt_flags_comp.sh
```

### Manual Way (now deprecated)
```
cd timing
```

the following benchmarks are interesting to compare:
- `BENCH_TYPE_POW_CURVE`
- `BENCH_TYPE_CHI`
- `BENCH_TYPE_CURVE_1174_POW`
- `BENCH_TYPE_CURVE_1174_CHI`
- `BENCH_TYPE_ELLIGATOR1_STR2PNT`
- `BENCH_TYPE_ELLIGATOR1_PNT2STR`

Get their codes as follows:
```bash
../scripts/gen_types.py \
    --src_files \
        src/runtime_benchmark_curve1174.c \
        src/runtime_benchmark.c \
    --strip_prefix \
        "BIGINT_TYPE_" \
    --lookup_names \
        BENCH_TYPE_POW_CURVE \
        BENCH_TYPE_CHI \
        BENCH_TYPE_CURVE_1174_POW \
        BENCH_TYPE_CURVE_1174_CHI \
        BENCH_TYPE_ELLIGATOR1_STR2PNT \
        BENCH_TYPE_ELLIGATOR1_PNT2STR
```

For the followin, choose `SETS` and `REPS` appropriately and then use the same value for all commands!

Create performance measurements using the following command
- replace `TODO2` with the previous benchmark integers, the types not in one of the benchmark will just be ignore
- replace `TODO1` with different optimization flags that you want to compare

```bash
OPT_FLAGS="<TODO1>" BENCHMARKS="<TODO2>" SETS=10 REPS=1000 make run-runtime-benchmark
OPT_FLAGS="<TODO1>" BENCHMARKS="<TODO2>" SETS=10 REPS=1000 make run-runtime-benchmark-curve1174
```

Take not of the log folders, and copy together the logs for the same optimization flag into their own folder.

Make a comparison plot as follows:
```bash
LOGS_NAMES="<TODO1>" LOGS_DIRS="<path/to/first/TODO1> </path/to/second/TODO1>" SETS=10 REPS=1000 make plot-comparison
```

## Compile Everything Together
It is very common for Makefiles to compile individual files to object files and then link them together to an executable. In large projects, the advantage is that you only need to recompile those object files that changed, which makes recompiling the project a lot faster. However, this is a potential issue for performance optimizations, since the compiler can only optimize in individual files and not over the entire project.

I changed the makefiles to compile everything together instead of using object files.

## Pow
- Remove copies by unrolling loop twice (done by Freya).
- Avoid square operations by early exit when no bits are left in the exponent

Pow for special exponent:
- Remove `b * 1` and `b * b` and instead use square function and set r directly to `b * b^2`
- Remove all copies by usual loop unrolling and clever variable juggling

## Roofline Plots
- Intel Manual: table 2-13 and 2-14 show the following exec units and ports:
    - ALU: 0, 1, 5, 6
    - SHIFT: 0, 6
    - Divide: 0
    - Slow int: 1
- Memory bandwidth:
    - Theoretical: probably < 30 GB/s ([src](https://apple.stackexchange.com/questions/276543/finding-peak-memory-bandwidth-on-macbook-pro))
    - Practical: 24.833 GB/s (measured with Novabench)
        - 2.8 GHz processor, so that gives:
        ```
            24.833 GB/s / 2.8 GHz = 8.868... ~= 8.9 B/cycle
        ```
- Peak performance:
    - Normal: 4 iops per cycle, since we have 4 ALUs behind different ports
    - Vectorized: 16 iops per cycle, since we mostly operate on 64 bit integers. One vector has 4 operands.
    - Depending on instruction mix:
        - The script `./scripts/make_elligator_stats_comp.sh` prints the instruction mix
        - Use this to add another peak performance line
- Intensity:
    - Work W: We count the ops, annotated by our macros
    - Transferred bytes Q:
        - This is pretty small if we only look at the function inputs.
        - map: 1 BigInt + curve = `12'672 B`
        - inv map: 1 point + curve = `13'824 B`
        - Sizes:
            - BigInt: `64B + 17 * 64 B = 1152 B` (64B metadata, 17 chunks with 64B)
            - curve: 10 BigInts, i.e., `11520 B`
            - point: 2 BigInts, i.e., `2304 B`

## Create mem ops plot
Command:
```
MAX_VERSION=2 PATTERN=".*(basic_add_chunk|alloc|memcpy|destroy|copy).*" ./scripts/make_elligator_stats_comp.sh
```

## Commands to run for final plots
Comparison V1 to V3:
```
SKIP_COMP_TO_GMP=1 SETS=10 REPS=10000 ./scripts/make_elligator_comp.sh
```

Comparison V3 to gmp:
```
MIN_VERSION=3 SETS=10 REPS=10000 ./scripts/make_elligator_comp.sh
```

Speedup V1 to V2 of using Curve1174-specific functions
```
SETS=10 REPS=10000 ./scripts/make_curve1174_vs_bigint_comp.sh
```

Plot to show benefit of using AVX for adds:
```
$ ../scripts/gen_types.py --src_file src/runtime_benchmark_curve1174.c  src/runtime_benchmark.c --dest_file include/benchmark_types.h --lookup_names BENCH_TYPE_ADD_256 BENCH_TYPE_ADD_GENERAL BENCH_TYPE_ADD_UPPER_BOUND
2 3 6

$ SETS=100 REPS=100000 BENCHMARKS="2 3 6" VERSION=3 make plot-runtime-benchmark
```

Plot heap vs. stack hacked back to v2.0:
- Checkout branch `v2.0_backported_plotting`
- Run `SKIP_COMP_TO_GMP=1 SETS=10 REPS=10 MAX_VERSION=2 ./scripts/make_elligator_comp.sh`
- Generate plots manually, as follows:
    - Runtime:
        ```
        ./scripts/gen_runtime_plots.py --title "Heap vs Stack Runtime Comparison for Elligator (10 sets, 10000 reps)" --plot_fname /Users/MiroETH/Documents/ETH/Msc/Semester_4/Advanced_Systems_Lab_Pueschel_263-0007-00/Project/team36/timing/plots/presentation/heap_vs_stack_elligator_comparison_bar_log_scale.png --logs_dir "/Users/MiroETH/Documents/ETH/Msc/Semester_4/Advanced_Systems_Lab_Pueschel_263-0007-00/Project/tmp/team36/timing/logs/V1/runtime/2021-06-07_09-15-21;/Users/MiroETH/Documents/ETH/Msc/Semester_4/Advanced_Systems_Lab_Pueschel_263-0007-00/Project/tmp/team36/timing/logs/V2/runtime/2021-06-07_09-24-04"  --logs_names "heap;stack" --bar_plot --log_yaxis
        ```
    - Speedup:
        ```
        ./scripts/gen_runtime_plots.py --title "Heap vs Stack Speedup for Elligator (10 sets, 10000 reps)" --plot_fname /Users/MiroETH/Documents/ETH/Msc/Semester_4/Advanced_Systems_Lab_Pueschel_263-0007-00/Project/team36/timing/plots/presentation/heap_vs_stack_elligator_speedup.png --logs_dir "/Users/MiroETH/Documents/ETH/Msc/Semester_4/Advanced_Systems_Lab_Pueschel_263-0007-00/Project/tmp/team36/timing/logs/V1/runtime/2021-06-07_09-15-21;/Users/MiroETH/Documents/ETH/Msc/Semester_4/Advanced_Systems_Lab_Pueschel_263-0007-00/Project/tmp/team36/timing/logs/V2/runtime/2021-06-07_09-24-04"  --logs_names "heap;stack" --speedup_plot
        ```

Laptop comparison:
- For instructions on which commands to run, see [here](./computer_comp.md)
- Manually create this folder structure (for `pnt2str`, the same for `str2pnt`):
    - `logs/V1_to_V3/pnt2str/V1/`
    - `logs/V1_to_V3/pnt2str/V2/`
    - `logs/V1_to_V3/pnt2str/V2/`
- Change the data label of each log file to the identifier (name, laptop model)
- Plot manually with the following commands
    - Runtime:
        ```
        ./scripts/gen_runtime_plots.py --title "Laptop Runtime Comparison for Elligator str2pnt (10 sets, 10000 reps)" --plot_fname ./plots/presentation/laptop_runtime_comparison_elligator_str2pnt.png --logs_dir "logs/V1_to_V3/str2pnt/V1;logs/V1_to_V3/str2pnt/V2;logs/V1_to_V3/str2pnt/V3"  --logs_names "V1;V2;V3" --bar_plot --log_yaxis

        ./scripts/gen_runtime_plots.py --title "Laptop Runtime Comparison for Elligator pnt2str (10 sets, 10000 reps)" --plot_fname ./plots/presentation/laptop_runtime_comparison_elligator_pnt2str.png --logs_dir "logs/V1_to_V3/pnt2str/V1;logs/V1_to_V3/pnt2str/V2;logs/V1_to_V3/pnt2str/V3"  --logs_names "V1;V2;V3" --bar_plot --log_yaxis
        ```
    Speedup:
        ```
        ./scripts/gen_runtime_plots.py --title "Laptop Speedup Comparison for Elligator str2pnt (10 sets, 10000 reps)" --plot_fname ./plots/presentation/laptop_speedup_comparison_elligator_str2pnt.png --logs_dir "logs/V1_to_V3/str2pnt/V1;logs/V1_to_V3/str2pnt/V2;logs/V1_to_V3/str2pnt/V3"  --logs_names "V1;V2;V3" --speedup_plot

        ./scripts/gen_runtime_plots.py --title "Laptop Speedup Comparison for Elligator pnt2str (10 sets, 10000 reps)" --plot_fname ./plots/presentation/laptop_speedup_comparison_elligator_pnt2str.png --logs_dir "logs/V1_to_V3/pnt2str/V1;logs/V1_to_V3/pnt2str/V2;logs/V1_to_V3/pnt2str/V3"  --logs_names "V1;V2;V3" --speedup_plot
        ```
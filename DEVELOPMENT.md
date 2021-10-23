# Development
## Folder structure
This project is organized into the following folders:
- `src`: source code of this project.
- `tests`: testing infrastructure run by `make test` and the CI pipeline.

## Makefiles
This project uses Makefiles. A quick introduction to Makefiles can be found [here](https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/).

The following commands are in general supported:
- `make`: Compile the code in the current folder.
- `make clean`: Remove compilation artifacts, temporary files, and executables from the current folder. In general, it should always be possible to run this command without loosing any information that cannot be regenerated easily.
- `make test`: Run all tests in `$REPO_SRC/tests`.

## Testing
We use `Check` for unit tests for `C`. To run the tests, you need to install `Check` as described [on their website](https://libcheck.github.io/check/web/install.html). There is also a [reference](https://libcheck.github.io/check/doc/check_html/index.html) for how to write unit tests with `check`.

### Adding a new test case
To add a new test case, do the following:

- If there is already a test suite for this feature, add your test to the existing C file in [tests/src](tests/src). The basic test structure is the following:
```C
START_TEST(test_<your test name>)
{
    /* your test */
}
END_TEST
```
Next, you need to add your test case to the test suite (which is defined in the corresponding `*_suite` function):
```C
tcase_add_test(tc_basic, test_<your test name>);
```
For more information on `Check` unit tests, look at the [reference](https://libcheck.github.io/check/doc/check_html/index.html).
- If you want to add a new test suite, create a new C file in [tests/src](tests/src). For the format you can look at already existing files.
    **Important**: You need to add this new test file to the Makefile. First, add the following to compile it:
    ```Makefile
    # Specify the object files constituting the test
    _OBJ	= <test suite name>.o
    OBJ 	= $(patsubst %,$(ODIR)/%,$(_OBJ))
    # Specify the object files of the tested functions
    _OBJ_TESTED = <tested files>.o
    OBJ_TESTED 	= $(patsubst %,$(ODIR_TESTED)/%,$(_OBJ_TESTED))

    .PHONY: compile-<test suite name>
    compile-<test suite name>: all-tested <test suite name>

    <test suite name>: $(OBJ_TESTED) $(OBJ)
    	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)
    ```
    Replace `<test suite name>` with the name of the new C file (for which it would make sense to name it after the test suite). Replace `<tested files>` with the files from [src](./src) which are to be tested (thus, need to be compiled as well).
    Finally, add `compile-<test suite name>` to the `all` target in the Makefile:
    ```Makefile
    all: [...] compile-<test suite name>
    ```

### Running all tests locally
Execute `make run-tests` from the [repository root](./) or the [tests](./tests) folder.

## Code Quality
### Formating
Use the following coding conventions (to be extended/discussed):
- 4 spaces instead of taps
- `snake_case` for function and variable names.
- Max. 80 characters on a single line.

### Function Annotation
Use the following template to briefly describe your functions in the code. See also the code examples.
```
/**
* Put a *brief* description of the function here.
*
* @param vname describe parameters that the function takes as input.
*
* @return describe the return value of the function.
*/
```

## Debugging
There are a bunch of helpful debug macros in `include/debug.h` for easier debug output, color coded macros, and support for log levels. There are the following log levels available:

0. quiet; no output
1. fatal; only get fatal errors
2. error; additionally, get non-fatal errors
3. warnings; additionally, get warnings
4. success; additionally, print successful statements
5. vebose; get general debug output

Set the log level in `include/debug.h`. Only output from the set level and lower levels will be printed.

To print to those levels, use the following functions: `FATAL`, `ERROR`, `WARNING`, `SUCCESS`, `DEBUG`. They all use normal `printf` syntax (format string followed by arguments).

### Debugging BigInts
The function `DEBUG_BIGINT` can be used to easily print BigInts for debugging. It takes as arguments a `bigint` followed by a format string. It can only print one bigint per call.

Example:
```C
DEBUG_BIGINT(a, "For i = %d, we have a = ", i);
```
Prints the following (e.g., for `i = 42`, `a = 0x102CC2711CB04A42`):
```text
For i = 42, we have a = 0x102CC271 1CB04A42
```

## Benchmarking
The code for benchmarks is located in `timings/*`.

Include the header file `benchmark_helpers.h` to get run benchmarks. It provides the following functionality:

- `benchmark_runner`: This function takes a `BenchmarkClosure` (see below), a title, log file name, number of sets `num_sets`, and number of repetitions `num_reps`. It then prints a benchmark header (see `printf_bench_header`), does warmup instructions, and executes the benchmarked function in `num_sets` measurements, each taking the average time measurement over `num_reps` function calls.
- `BenchmarkClosure` consists of three functions and their arguments:
    - `bench_prep_fn`: Called for each set before the function that we want to benchmark. Use this to set up variables.
    - `bench_fn`: Function to benchmark. Invocations of this function will be timed.
    - `bench_cleanup_fn`: Called after each set. Use this to cleanup any data structures that were allocated for the test.
    - `bench_prep_args`: A `void *` point as argument for `bench_prep_fn`. This should be casted to a pointer to whatever arguments `bench_prep_fn` needs.
    - `bench_fn` and `bench_cleanup_fn` have no arguments yet, but this could be added similar to `bench_prep_args`.

For an example, look at the Chi Function Benchmark in `timings/src/bigint_benchmark.c`.

### Add New Benchmark
To add a new benchmark, first add benchmarking code in `src` or generate log files in a different way. Then add the plotting command to `scripts/make_all_plots.sh`.

### Running Single Benchmarks
It is possible to run single benchmarks by calling the benchmark with a list of the benchmarks to execute. The available benchmarks are described in `timing/include/benchmark_types.h` (this file is auto-generated from `runtime_benchmark.c` for better maintainability). The list should be a string with space-separated numbers. The numbers can be in any order. Alternatively, the environment variable `BENCHMARKS` can be used with the Makefile. The special argument `-1` executes all tests. This is the default in the Makefile.

Examples:
```
./timing/bin/runtime-benchmark 21 1 2

cd timing && BENCHMARKS="12 1 13" make plots
```

### Plotting
Plots can be generated from logs (usually generated by the above benchmarking functions) using scripts in `timing/scripts`. The simplest way to generate standard plots is to over the make file; run in `timing/` the following commands:
- `make plots`: Will compile and run all benchmarks specified in `BENCHMARKS` (or all existing ones if `BENCHMARKS` is not set or set to `-1`), then create all plots.

Set the environment variable `PUBLISH` to a folder name under which the plots should be published. They will then be added to the folder `timing/plots/$(PUBLISH)/{runtime, stats}/`, which will be checked in by git.

#### Prerequisites
Install `matplotlib` for `pip3`. One way to install all packages needed for this repo is to run in the root of this repository:
```
pip3 install --user -r pip_requirements.txt
```

#### Comparison Plots
To make comparisons plots, you can give multiple logs dirs to the `timing/scripts/gen_runtime_plots.py` script. Or even easier, use the `make plot-comparison` target. Gather the paths to the log folders that should be compared and put them in a environment variable called `LOGS_DIRS`. Next, name those data sets with a short name by setting the variable `LOGS_NAMES` in the same order. Don't forget to use the same `SETS` and `REPS` for generating all log files and also specify them for the comparison plot.
```
LOGS_NAMES="gmp V1 V2" LOGS_DIRS="./logs/gmp/<etc> ./logs/V1/<etc> ./logs/V2/<etc>" SETS=10 REPS=1000 make plot-comparison
```

#### Help Pages
You can also directly use the plotting scripts. They have the following options.

Help page of `gen_perf_plot.py`:
```text
usage: gen_perf_plot.py [-h] [--plot_fname PLOT_FNAME] [--title TITLE] [--log_xaxis]
                        [--log_yaxis]
                        in_files [in_files ...]

positional arguments:
  in_files              One or more input files to plot. Input files must have the
                        following structure: Line 1: Data label; Line 2: 'x-label,
                        y-label' Line 3-*: 'x, y' values (separated by comma and
                        space)

optional arguments:
  -h, --help            show this help message and exit
  --plot_fname PLOT_FNAME
                        Path of the generated plot.
  --title TITLE         Title for the generated plot.
  --log_xaxis           Toggle x-axis to have a log scale.
  --log_yaxis           Toggle y-axis to have a log scale.
```

Help page of `gen_roofline_plot.py`:
```text
usage: gen_roofline_plot.py [-h] [--title TITLE] [--no_border]
                            [--plot_fname PLOT_FNAME]
                            in_files [in_files ...]

positional arguments:
  in_files              One or more input files to plot. Input files must have the
                        following structure: Line 1: peak performance values as val,
                        label; val, label;Line 2: bandwidth;Line 3: max intensity (for
                        x-axis);Line 4-*: dots in the format: dot label, x, ythe y
                        value can be max(a,b,...), where the arguments in the bracket
                        declare which peak performance bounds should be considered.
                        Shorthand: max to consider all

optional arguments:
  -h, --help            show this help message and exit
  --title TITLE         Title for the generated plot.
  --no_border           Do not plot the border between memory and compute bounded
                        areas
  --plot_fname PLOT_FNAME
                        Path of the generated plot.
```

## Profiling
### Count function calls
There is the option to collect statistics on which functions were called. For that purpose, compile the BigInt library with the environment variable `COLLECT_STATS` set. One can use the benchmarking code to get the stats of single function invocations. for that, run the following in the `timing` subfolder:
```
BENCHMARKS=14 COLLECT_STATS=1 make plots
```
Where `BENCHMARKS` is set to the benchmarks from which the stats should be collected. Each of them gets its own file with the stat count for every BigInt function.

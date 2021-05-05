# Development
## Folder structure
This project is organized into the following folders:
- `src`: source code of this project.
- `tests`: testing infrastructure run by `make test` and the CI pipeline.

## Git
### Initial Setup
Don't forget to configure your name and email before you commit your first changes.
```
git config --global user.name "<YOUR NAME>"
git config --global user.email "<your eth username>@student.ethz.ch"
```

### Branching Model
The idea is to use the following simple branching model:
- **master**: this branch contains a _stable_ implementation (like the production environment in practice). You should never directly work on the _master_ branch.
- **dev**: this branch is intended for development. This branch should be used as source to create branches for new features. Completed features should be merged back into this branch. When a set of features is done and stable, this branch is merged back to _master_.
- **feature-branches**: those are branches that you should mainly work on. Whenever it makes sense, everyone should work on his own feature branch. Those branches should have descriptive names. Once a feature is done, you should create a _merge request_ to push it to the _dev_ branch. Every merge request must be reviewed by another team member. Moreover, it is only allowed to merge back if the CI pipeline ran successfully.
- **hotfix**: this is the exception to the rule. In case something is broken on the _master_ branch, which has to be fixed immediately, you can create a  branch directly from the _master_, apply the change and directly merge it back to the _master_ branch.

### .gitignore
We have a pretty restrictive `.gitignore` file. In case some of your changes do not appear on git status (i.e., you cannot commit them), check all `.gitignore` files, especially [this one](./.gitignore).

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

### Plotting
Plots can be generated from logs (usually generated by the above benchmarking functions) using scripts in `timing/scripts`. The simplest way to generate standard plots is to over the make file; run in `timing/` the following commands:
- `make plots`: Will compile and run all benchmarks, then create all plots.
- `make publish-plots`: Same as `make plots` but stores the plots directly in `timing/plots`. Those are tracked by git and thus should only be used when you want to commit a new plot.

#### Prerequisites
Install `matplotlib` for `pip3`. One way to install all packages needed for this repo is to run in the root of this repository:
```
pip3 install --user -r pip_requirements.txt
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

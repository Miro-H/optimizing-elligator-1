#ifndef BENCHMARK_H_    /* Include guard */
#define BENCHMARK_H_

#define WARMUP_CYCLES 1000000

typedef struct BenchmarkClosure {
    void *bench_prep_args;
    void (*bench_prep_fn)(void *);
    void (*bench_fn)(void *);
    void (*bench_cleanup_fn)(void *);
} BenchmarkClosure;

void benchmark_runner(BenchmarkClosure bench_closure, char *bench_name,
    char *log_fname, uint64_t num_sets, uint64_t num_reps, uint64_t used_values);

void benchmark_runner_always_set_up_and_clean(BenchmarkClosure bench_closure, char *bench_name,
    char *log_fname, uint64_t num_sets, uint64_t num_reps, uint64_t num_internal_reps);

void printf_bench_header(const char *fmt, ...);

#define BENCHMARK(arg, type, bench_fn)                  \
    do {                                                \
        if ((arg) == (type) || (arg) == -1) {           \
            bench_fn;                                   \
        }                                               \
    } while(0);

#endif // BENCHMARK_H_

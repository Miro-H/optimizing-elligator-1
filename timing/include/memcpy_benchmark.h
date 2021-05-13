#ifndef MEMCPY_BENCHMARK_H_ /* Include guard */
#define MEMCPY_BENCHMARK_H_

#ifndef LOG_PATH
#define LOG_PATH "../logs/memcpy"
#endif

#ifndef BIGINTSIZE
// 256 bits
#define BIGINTSIZE 8
#endif

#ifndef SETS
#define SETS 100
#endif

#ifndef REPS
#define REPS 10000
#endif

/*
 * Global variables
 */
void **mem_array;
int64_t mem_elem_size;

#endif // MEMCPY_BENCHMARK_H_

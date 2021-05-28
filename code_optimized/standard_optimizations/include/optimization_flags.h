#ifndef OPT_FLAGS_H_    /* Include guard */
#define OPT_FLAGS_H_

// Often used macros to provide hint of likely branches to compiler
#define LIKELY(x)    __builtin_expect (!!(x), 1)
#define UNLIKELY(x)  __builtin_expect (!!(x), 0)

#endif // OPT_FLAGS_H_

/*
 * This file is adapted from the "tsc_x86.h" file handed out to us for the
 * homeworks of ASL 2021.
 *
 * Short description of this file:
 * This file implements TSC timers for non-windows systems.
 */

#ifndef TSC_X86_H_    /* Include guard */
#define TSC_X86_H_

#include <stdint.h>

#if defined(WIN32)
    #error "Windows is not supported for benchmarking"
#endif

/* ================ GNU C and possibly other UNIX compilers ================= */
#if !defined(WIN32) || defined(__GNUC__)
	#if defined(__GNUC__) || defined(__linux__)
		#define VOLATILE __volatile__
		#define ASM __asm__
	#else
        #warning "Unknown architecture, try speculatively guessed macros"

		/* if we're neither compiling with gcc or under linux, we can hope
		 * the following lines work, they probably won't */
		#define ASM asm
		#define VOLATILE
	#endif
#endif

/* This is the RDTSC timer.
 * RDTSC is an instruction on several Intel and compatible CPUs that Reads the
 * Time Stamp Counter. The Intel manuals contain more information.
 */

#define COUNTER_LO(a) ((a).uint32.lo)
#define COUNTER_HI(a) ((a).uint32.hi)
#define COUNTER_VAL(a) ((a).uint64)

#define COUNTER(a) \
	((unsigned long long)COUNTER_VAL(a))

#define COUNTER_DIFF(a,b) \
	(COUNTER(a)-COUNTER(b))

/* ==================== GNU C and possibly other UNIX compilers ===================== */
#if !defined(WIN32) || defined(__GNUC__)
    typedef union
    {
        uint64_t uint64;
        struct {
            uint32_t lo, hi;
        } uint32;
    } tsc_counter;

    #define RDTSC(cpu_c) \
            ASM VOLATILE ("rdtsc" : "=a" ((cpu_c).uint32.lo), "=d"((cpu_c).uint32.hi))

    #define CPUID() \
            ASM VOLATILE ("cpuid" : : "a" (0) : "bx", "cx", "dx" )
#endif

uint64_t start_tsc(void) {
    tsc_counter start;
    CPUID();
    RDTSC(start);
    return COUNTER_VAL(start);
}

uint64_t stop_tsc(uint64_t start) {
	tsc_counter end;
	RDTSC(end);
	CPUID();
	return COUNTER_VAL(end) - start;
}

#endif // TSC_X86_H_

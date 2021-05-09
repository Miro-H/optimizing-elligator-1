/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This is an example C file that should serve as a template for the real
 * implementation.
 */

/*
 * Includes
 */
#include <stdio.h>

// Include header files
#include "example.h"

/*
 * Defines
 */
#define LOCAL_MACRO 1

/*
 * Function prototypes
 */

/**
* Local function to call all other functions. Showcases a local function.
*/
void call_all(void);

/*
 * Function Declarations
 */
void call_all(void) {
    get_local_macro();
    get_global_str();
}

int get_local_macro() {
    printf("Local macro: %d\n", LOCAL_MACRO);
    return LOCAL_MACRO;
}

const char *get_global_str() {
    printf("Global string from header file: %s\n", GLOBAL_STR);
    return GLOBAL_STR;
}

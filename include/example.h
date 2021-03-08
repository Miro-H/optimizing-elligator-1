/*
 * This file is part of the ASL project "Censorship-avoiding high-speed EC
 * (Elligator with Curve1174)"
 * (https://gitlab.inf.ethz.ch/COURSE-ASL/asl21/team36).
 *
 * Short description of this file:
 * This is an example header file belonging to the example.c C file. It should
 * serve as a template for the real implementation.
 */

#ifndef HEADER_EXAMPLE_H
#define HEADER_EXAMPLE_H

#define GLOBAL_STR "global test"

/**
* Example function to print and return a string from example.h
*
* @return constant string from example.h
*/
int get_local_macro(void);

/**
* Example function to print and return a string from example.h
*
* @return constant string from example.h
*/
const char *get_global_str(void);

#endif // HEADER_EXAMPLE_H

/**
 * @file cuf_util.h
 * @brief CUnitFramework (CUF): Utilities Interface
 * @details Misc utilities for the CUF library, including some cleanup functions
 * and other bits and bobs for easier test writing
 */
#ifndef __CUF_UTIL_H__
#define __CUF_UTIL_H__

#include <stdlib.h>

#include "cuf.h"

/**
 * Declare a variable as unused to suppress warnings
 */
#define CUF_UNUSED(a) (void)(a);

/**
 * Declare a array comparison function for use with the assertions
 * 
 * @param name name of function
 * @param T type of array elemts being compared
 */
#define ARRAY_COMP_FUNC(name, T) bool name(int i, T a, T b,\
                                         char **emsgs, size_t *eused,\
                                         size_t* esize)

/**
 * Expand dynamically reallocated array
 * 
 * @param a dynamic array to expand
 * @param n size of a
 * @param factor factor to expand a by
 * @param t type of the elements of a
 */
#define CUF_ARRAY_EXPAND(a, n, factor, t) do {\
    n *= factor;\
    a = (t) realloc(a, n*sizeof(t));\
} while(0)

/**
 * Helpful dependency cleanup function to run after suite terminates
 * 
 * @param suite pointer to current TestSuite.
 */
SUITE_TERM_FUNC(term_cleanup_deps);

/**
 * Helpful argument cleanup function to run after suite terminates
 * 
 * @param suite pointer to current TestSuite
 */
SUITE_TERM_FUNC(term_cleanup_args);

#endif

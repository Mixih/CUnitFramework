/**
 * @file cuf_util.c
 * @brief CUnitFramework (CUF): Utilities Implementation
 * @details Misc utilities for the CUF library, including some cleanup functions
 * and other bits and bobs for easier test writing
 */
#include "cuf_util.h"

SUITE_TERM_FUNC(term_cleanup_deps) {
    for(int i = 0; i < suite->test_count; ++i) {
        if(suite->testcases[i]->deps) {
            dependency_destroy(suite->testcases[i]->deps);
            suite->testcases[i]->deps = NULL;
        }
    }
}

SUITE_TERM_FUNC(term_cleanup_args) {
    for(int i = 0; i < suite->test_count; ++i) {
        if(suite->testcases[i]->args) {
            free(suite->testcases[i]->args);
            suite->testcases[i]->args = NULL;
        }
    }
}

/**
 * @file cuf_assert.h
 * @brief CUnitFramework (CUF): Assertions
 * @details The assertion macros at the core of this library. Please note that
 * these will not work without the supporting CUF testing infrastructure, i.e.
 * you must use these from within a TESTCASE definition, executed by a
 * TestRunner for everything to work correctly.
 */
#ifndef __CUF_ASSERT_H__
#define __CUF_ASSERT_H__

#include "cuf.h"
#include "cuf_util.h"


/**
 * Assert that a is TRUE, and fail the test if this is not true
 * 
 * @param a value to assert
 */
#define ASSERT_TRUE(a) do {\
    if(a != true) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` should be TRUE\nat %s:%d; in TestCase:"\
                    " %s", #a, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is FALSE, and fail the test if this is not true
 * 
 * @param a value to assert
 */
#define ASSERT_FALSE(a) do {\
    if(a == true) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` should be FALSE\nat %s:%d; in TestCase:"\
                    " %s", #a, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is equal to b, and fail the test if this is not true
 * 
 * @param a LHS of comparison
 * @param b RHS of comparison
 */
#define ASSERT_EQ(a,b) do {\
    if(!(a == b)) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` and `%s` should BE EQUAL\n"\
                    "At %s:%d; in TestCase: %s", #a, #b, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name);\
        testsuite_record_fail(suite, (char *)&msg);\
    }\
} while (0)
/**
 * Assert that a is NOT EQUAL to b, and fail the test if this is not true
 * 
 * @param a LHS of comparison
 * @param b RHS of comparison
 */
#define ASSERT_NE(a,b) do {\
    if(a == b) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` and `%s` should NOT BE EQUAL\n"\
                    "at %s:%d; in TestCase: %s", #a, #b, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name]);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is LESS THAN to b, and fail the test if this is not true
 * 
 * @param a LHS of comparison
 * @param b RHS of comparison
 */
#define ASSERT_LT(a,b) do {\
    if(a >= b) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` should be LESS THAN `%s` \n"\
                    "At %s:%d; in TestCase: %s", #a, #b, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name]);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is LESS THAN or EQUAL to b, and fail the test if this is not
 * true
 * 
 * @param a LHS of comparison
 * @param b RHS of comparison
 */
#define ASSERT_LE(a,b) do {\
    if(a > b) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` should be LESS THAN or EQUAL to `%s`\n"\
                    "at %s:%d; in case: %s", #a, #b, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name]);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is GREATER THAN b, and fail the test if this is not true
 * 
 * @param a LHS of comparison
 * @param b RHS of comparison
 */
#define ASSERT_GT(a,b) do {\
    if(a <= b) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` should be GREATER THAN to `%s`\n"\
                    "at %s:%d; in case: %s", #a, #b, __FILE__, __LINE__,\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is GREATER THAN or EQUAL to b, and fail the test if this is not
 * true
 *
 * @param a LHS of comparison
 * @param b RHS of comparison
 */
#define ASSERT_GE(a,b) do {\
    if(a < b) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "Value of `%s` should be GREATER THAN or EQUAL to `%s`\n"\
                    "At %s:%d; in TestCase: %s", #a, #b, __FILE__, __LINE__,\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)

/**
 * Assert that a is NULL
 * 
 * @param a variable/value to check
 */
#define ASSERT_NOT_NULL(a) do {\
    if(!a) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "`%s` is NULL or NULL pointer\nAt %s:%d; in TestCase:"\
                    " %s", #a, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)
/**
 * Assert that a is NOT NULL
 * 
 * @param a variable/value to check
 */
#define ASSERT_NULL(a) do {\
    if(a) {\
        char msg[CUF_BUF_SIZE] = {'0'};\
        snprintf((char *) &msg, CUF_BUF_SIZE, "Assertion failure: "\
                    "`%s` is NOT NULL or NULL pointer\nAt %s:%d; in TestCase:"\
                    " %s", #a, __FILE__, __LINE__,\
                    suite->testcases[suite->current_test]->test_name);\
        testsuite_record_fail(suite, (char *) &msg);\
    }\
} while (0)

/**
 * Assert values in an n-dimentional array against an array of expected values,
 * using a comparison function `comp_func`
 *
 * @param actual values to compare against reference
 * @param expected reference array
 * @param comp_func function identifier to use for comparison. Must be callable!
 * @param n number of element sin array
 */
#define ASSERT_ARRAY(actual, expected, comp_func, n) do {\
    int cuf_arr_errors = 0;\
    char* cuf_cfm = malloc(CUF_BUF_SIZE);\
    size_t cuf_cfm_used = 0;\
    size_t cuf_cfm_size = CUF_BUF_SIZE;\
    for(size_t i = 0; i < (size_t) n; ++i) {\
        if(cuf_arr_errors >= CUF_ERR_LIMIT) {\
            if(cuf_cfm_size - cuf_cfm_used <= 60) {\
            CUF_ARRAY_EXPAND(cuf_cfm, cuf_cfm_size, 2, char*);\
            }\
            char cuf_trunc_msg[] = "Errors exceeded max output... Truncated...";\
            strncpy(&cuf_cfm[cuf_cfm_used], cuf_trunc_msg, strlen(cuf_trunc_msg));\
            cuf_cfm_used += strlen(cuf_trunc_msg);\
            break;\
        }\
        bool cuf_arr_status = comp_func(i, &actual[i], &expected[i], &cuf_cfm, &cuf_cfm_used,\
                                        &cuf_cfm_size);\
        if(cuf_arr_status == false) {\
            ++cuf_arr_errors;\
        }\
    }\
    cuf_cfm[cuf_cfm_used] = '\0';\
    if(cuf_arr_errors > 0) {\
        char *msg = malloc((CUF_BUF_SIZE + cuf_cfm_used + 1)*sizeof(char));\
        snprintf(msg, CUF_BUF_SIZE + cuf_cfm_used + 1, "Assertion failure:"\
                " array comparison of `%s` against `%s` with comparison function `%s` failed"\
                "\nAt %s:%d; in TestCase: %s\nFail Elems:\n%s", #actual, #expected, #comp_func,\
                __FILE__, __LINE__, suite->testcases[suite->current_test]->test_name,\
                cuf_cfm);\
        testsuite_record_fail(suite, msg);\
        free(msg);\
    }\
    free(cuf_cfm);\
} while (0)

#endif

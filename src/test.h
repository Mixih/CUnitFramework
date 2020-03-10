// Custom CUnitFramework v0.1a
// Inspired by the piles of unit testing frameworks I've used over the years
// Note: Must define "uut.h" for pluggable uut functionality
#ifndef __TEST_H__
#define __TEST_H__
#include <stdbool.h>
#include <stdio.h>

#include "uut.h"

/**
 * Define a unit under test "object" with the given name
 * 
 * @param name name of the "object"
 */
#define DEFINE_UUT(name) UUT name = {{'\0'}, NULL, NULL, NULL}
/**
 * Define a testcase function
 * 
 * @param name name of testcase, msut be valid c name and unique
 */
#define TESTCASE(name) void name(UUT *uut, TestSuite *suite)
/**
 * shortcut macro to register a testcase to a testsuite
 * 
 * @param suite TestSuite object to register testcase to
 * @param testcase TestFunc function to register
 * @param args argument object for given case
 */
#define REGISTER_TESTCASE(suite, testcase, file_deps, args)\
            testsuite_reg_case(suite, testcase, file_deps, #testcase, args)
/**
 * another handy macro to register test suties to test runner (TestRunner) objects
 * 
 * @param test TestRunner object to register the suite to
 * @param suite TestSuite object to register to the TestRunner object
 */
#define REGISTER_TESTSUITE(test, suite) testrunner_reg_suite(test, suite, #suite)
/**
 * Define a setup function, will be run before each testcase in a suite
 * 
 * @param name name of setup function, must be valid c name and unique
 */
#define SETUPFUNC(name) void name(UUT *uut, Args *args)
/**
 * Define a teardown function, will be run after each testcase in a suite
 * 
 * @param name name of teardown function, must be valid c name and unique
 */
#define TEARDOWNFUNC(name) void name(UUT *uut, Args *args)
// internal constants
#define TCD_ARRAY_SIZE 8
#define TCD_BUF_SIZE 250

/**
 * structure for testcase arguments that must change between cases in a suite
 */
struct args_t {
    bool del_input;
    char ifile[250];
    char ofile[250];
};

// Typedefs to make life easiser
typedef struct args_t Args;
typedef struct uut_t UUT;
typedef struct testcase_t TestCase;
typedef struct testsuite_t TestSuite;
typedef struct test_t TestRunner;
/**
 * a function pointer to a testcase function
 * 
 * @param uut custom uut object supplied to each case
 * @param suite testcuite that is running this function
 */
typedef void (*TestFunc) (UUT *uut, TestSuite *suite);
/**
 * a function pointer to a setup function
 * 
 * @param uut custom uut object supplied to each case
 * @param args custom args that change between cases within a suite
 */
typedef void (*SetupFunc) (UUT *uut, Args *args);
/**
 * a function pointer to a teardown function
 * 
 * @param uut custom uut object supplied to each case
 * @param args custom args that change between cases within a suite
 */
typedef void (*TeardownFunc) (UUT *uut, Args* args);

/**
 * Struct for individual testcases
 */
struct testcase_t {
    TestFunc testfunc;
    char *test_name;
    Args args;
    int status;
    char *file_deps;
    char **err_msgs;
    int err_msg_count;
    int err_size;
};

/**
 * Creates a a testcase on the heap and returns a pointer to it
 * 
 * @param funct TestFunc function to use when creating the TestCase
 * @param test_name name to call this test case
 * @param file_deps delimted string of all files this case depends on
 * @param args variable ARGs object to use for this case
 */
TestCase *testcase_create(TestFunc funct, char* test_name, char *file_deps, Args *args);
/**
 * deallocate dynamically allocated testcases
 * 
 * @param testcase testcase to testcase_destroy
 */
void testcase_destroy(TestCase *testcase);

/**
 * Struct to contain information about TestSuite isntances
 */
struct testsuite_t {
    TestCase **testcases;
    SetupFunc setup;
    TeardownFunc teardown;
    int arr_size;
    int test_count;
    int current_test;
    int passed;
    int failed;
    int skipped;
};

/**
 * Creates a testsuite on the heap and returns a pointer to it
 * 
 * @param setup setup function to call before every case
 * @param teardown teardown function to call before every case
 */
TestSuite *testsuite_create(SetupFunc setup, TeardownFunc teardown);
/**
 * Register function to testsuite
 * 
 * @param suite TestSuite object to register testcase to
 * @param test TestFunc object to register
 * @param file_deps newline delimited string of all files this case depends on
 * @param test_name name to call this test case
 * @param args argument object for given case
 */
int testsuite_reg_case(TestSuite *suite, TestFunc test, char *file_deps,
                       char *test_name, Args *args);
/**
 * Record a failure to current test. Internal use function.
 * 
 * Note: you shouldn't use this function directly. Call an assertation macro instead.
 * 
 * @param suite test suite object to record the failure to
 * @param err_msg error message to log
 */
int testsuite_record_fail(TestSuite *suite, char* err_msg);
/**
 * Run a single test suite and cllect results
 * 
 * @param suite suite to run
 */
int testsuite_run(TestSuite *suite);
/**
 * Deallocate a heap allocate testsuite and all child testcases
 * 
 * @param suite testsuite to deallocate
 */
int testsuite_destroy(TestSuite *suite);

//The following are the assertation macros, the core of this framework

/**
 * Assert that a is equal to b, and fail the test if this is not true
 */
#define ASSERT_EQUAL(a,b) do {\
            if(!(a == b)) {\
                char msg[TCD_BUF_SIZE] = {'0'};\
                snprintf((char *) &msg, TCD_BUF_SIZE, "Assertion failure: "\
                         "Values of %s and %s should BE EQUAL\nat %s:%d; in case:"\
                         " %s", #a, #b, __FILE__, __LINE__,\
                         suite->testcases[suite->current_test]->test_name);\
                testsuite_record_fail(suite, (char *)&msg);\
            }\
            } while (0)
/**
 * Assert that a is NOT EQUAL to b, and fail the test if this is not true
 */
#define ASSERT_NOTEQUAL(a,b) do {\
            if(a == b) {\
                char msg[TCD_BUF_SIZE] = {'0'};\
                snprintf((char *) &msg, TCD_BUF_SIZE, "Assertion failure: "\
                         "Values of %s and %s should NOT BE EQUAL\nat %s:%d; in case:"\
                         " %s", #a, #b, __FILE__, __LINE__,\
                         suite->testcases[suite->current_test]->test_name]);\
                testsuite_record_fail(suite, (char *)&msg);\
            }\
            } while (0)
/**
 * Assert that a is TRUE, and fail the test if this is not true
 */
#define ASSERT_TRUE(a) do {\
            if(a != true) {\
                char msg[TCD_BUF_SIZE] = {'0'};\
                snprintf((char *) &msg, TCD_BUF_SIZE, "Assertion failure: "\
                         "Value of %s should be TRUE\nat %s:%d; in case:"\
                         " %s", #a, __FILE__, __LINE__,\
                         suite->testcases[suite->current_test]->test_name);\
                testsuite_record_fail(suite, (char *)&msg);\
            }\
            } while (0)
/**
 * Assert that a is FALSE, and fail the test if this is not true
 */
#define ASSERT_FALSE(a) do {\
            if(a == true) {\
                char msg[TCD_BUF_SIZE] = {'0'};\
                snprintf((char *) &msg, TCD_BUF_SIZE, "Assertion failure: "\
                         "Value of %s should be FALSE\nat %s:%d; in case:"\
                         " %s", #a, __FILE__, __LINE__,\
                         suite->testcases[suite->current_test]->test_name);\
                testsuite_record_fail(suite, (char *)&msg);\
            }\
            } while (0)

/**
 * test runner "object"
 */
struct test_t {
    TestSuite **suites;
    char **suite_names;
    int arr_size;
    int suite_count;
    int current_suite;
};
/**
 * Creates a testrunner on the heap and returns a pointer to it
 */
TestRunner* testrunner_create();
/**
 * Register a TestSuite to the given testrunner
 * 
 * @param runner testrunner to register obejct to
 * @param TestSuite suite to register to runner
 * @param name name to call the test suite
 */
void testrunner_reg_suite(TestRunner *runner, TestSuite **suite, char *name);
/**
 * Run the given test runner and print results to stdout
 * 
 * @param runner testrunner to run
 */
int testrunner_run(TestRunner *runner);
/**
 * deallocate heap allocated test runner
 * 
 * @param runner testrunner object to testcase_destroy
 */
void testrunner_destroy(TestRunner *runner);
#endif

/** 
 * @file cuf.h
 * @brief CUnitFramework (CUF): Primary Public Interfaces
 * @details Inspired by the unit testing frameworks I've used over the years,
 * except much, much WORSE, since this was written over the span of two late
 * nights. This is a manual registry type unit testing framework since C simply
 * doesn't have a good way to autoreg testcases apart from super-hacky ELF
 * parsing or compiler hacks
 *
 * @version v0.1b
 */
#ifndef __TEST_H__
#define __TEST_H__
#include <stdbool.h>
#include <stdio.h>

#include "cuf_dep.h"


/**
 * Define a testcase function
 * 
 * @param name name of testcase, msut be valid c name and unique
 */
#define TESTCASE(name) void name(void *uut, TestSuite *suite)
/**
 * shortcut macro to register a testcase to a testsuite
 * 
 * @param suite TestSuite object to register testcase to
 * @param testcase TestFunc function to register
 * @param args argument object for given case
 */
#define REGISTER_TESTCASE(suite, testcase, deps, args)\
            testsuite_reg_case(suite, testcase, deps, #testcase, args)
/**
 * Another handy macro to register test suties to test runner (TestRunner) objects
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
#define SETUP_FUNC(name) void name(void **uut, void *args, TestCase *tc)
/**
 * Define a teardown function, will be run after each testcase in a suite
 * 
 * @param name name of teardown function, must be valid c name and unique
 */
#define TEARDOWN_FUNC(name) void name(void *uut, void *args, TestCase *tc)
/**
 * Define a suite initializaiton function, which will be run BEFORE ANY
 * testcases have been run in a suite
 * 
 * @param name name of suite init function, must be valid c name and unique
 */
#define SUITE_INIT_FUNC(name) void name(TestSuite *suite)
/**
 * Define a suite termination function, which will be run AFTER ALL testcases
 * have been run in a suite
 * 
 * @param name name of suite term function, must be valid c name and unique
 */
#define SUITE_TERM_FUNC(name) void name(TestSuite *suite)
// internal constants
#define CUF_ARRAY_SIZE 8
#define CUF_BUF_SIZE 250
#define CUF_ERR_LIMIT 10


// Typedefs to make life easiser
typedef struct testcase_t TestCase;
typedef struct testsuite_t TestSuite;
typedef struct testrunner_t TestRunner;
/**
 * a function pointer to a testcase function
 * 
 * @param uut custom uut object supplied to each case
 * @param suite testcuite that is running this function
 */
typedef void (*TestFunc) (void *uut, TestSuite *suite);
/**
 * a function pointer to a setup function, run before EACH testcase is run
 * 
 * @param uut custom uut object supplied to each case
 * @param args custom args that change between cases within a suite
 * @param tc pointer to current testcase
 */
typedef void (*SetupFunc) (void **uut, void *args, TestCase *tc);
/**
 * a function pointer to a teardown function, run AFTER EACH testcase is run
 *
 * @param uut custom uut object supplied to each case
 * @param args custom args that change between cases within a suite
 * @param tc pointer to current testcase
 */
typedef void (*TeardownFunc) (void *uut, void *args, TestCase *tc);
/**
 * a function pointer to a suite initialization function, run BEFORE ANY
 * testcases are run
 * 
 * @param suite pointer to current testsuite
 * 
 */
typedef void (*SuiteInitFunc) (TestSuite *suite);
/**
 * a function pointer to a suite initialization function, run AFTER ALL
 * testcases int eh suite have been run
 * 
 * @param suite pointer to current testsuite
 */
typedef void (*SuiteTermFunc) (TestSuite *suite);


/**
 * listing of valid state codes for testcases
 */
enum cuf_tc_codes {
    CUF_TC_SKIP = -2,      /**< code for a testcase that was skipped */
    CUF_TC_FAIL = -1,      /**< code for a testcase that had failed assertation */
    CUF_TC_PASS = 0        /**< code for a testcase that passed all asserts */
};


// TestCase object def
/**
 * Struct for individual testcases. Yes there are void pointers. That's the
 * price you pay for generics in C; make sure they get casted to proper structs.
 *
 * Note: You really shouldn't be trying to use this directly (because you'll
 * almost certainly cause a leak). Use one of the function below design to work
 * on these if you don't hate yourself.
 */
struct testcase_t {
    TestFunc testfunc;     /**< function to call run this test */
    char *test_name;       /**< string of the human name of this test */
    void *args;            /**< pointer to a object that hold changle params for this case */
    int status;            /**< status code of the test */
    Dependency *deps;      /**< Pointer to `Dependency` object initialized with deps*/
    char **err_msgs;       /**< list of errors registered by assert statements */
    int err_msg_count;     /**< number of of discrete error messages */
    int err_size;          /**< size of the err_msgs buffer */
};
// TestCase object manipulators
/**
 * Creates a testcase on the heap and returns a pointer to it
 *
 * @param funct TestFunc function to use when creating the TestCase
 * @param test_name name to call this test case
 * @param deps Pointer to `Dependency` object containing dependencies. Note: you
 *             must call dependency_destroy() on this object yourself, whether in
 *             teardown(), or somewhere else, otherwise you will have a leak.
 * @param args variable argss object to use for this case
 */
TestCase *testcase_create(TestFunc funct, char* test_name, Dependency *deps,
                          void *args);
/**
 * deallocate dynamically allocated testcases
 * 
 * @param testcase testcase to destroy
 */
void testcase_destroy(TestCase *testcase);

// TestSuite object def
/**
 * Struct to contain information about TestSuite isntances
 */
struct testsuite_t {
    TestCase **testcases;   /**< array of TestCase objects associated with this suite */ 
    SetupFunc setup;        /**< SetupFunc to associate with this case */
    TeardownFunc teardown;  /**< TeardownFunc to associate with this suite */
    SuiteInitFunc init;     /**< init function to associate with this suite */
    SuiteTermFunc term;     /**< termination funciton to associate with this suite */ 
    char *name;             /**< name of the suite */
    int arr_size;           /**< size of dynamic memory buffers (in number of elems) */
    int test_count;         /**< number of tests in suite */
    int current_test;       /**< index of current test being run */
    int passed;             /**< number of passed tests */
    int failed;             /**< number of failed tests */
    int skipped;            /**< number of skipped tests */
};
// TestSuite object manipulators
/**
 * Creates a testsuite on the heap and returns a pointer to it
 * 
 * @param setup setup function to call before every case
 * @param teardown teardown function to call before every case
 */
TestSuite *testsuite_create(char* name, SetupFunc setup, TeardownFunc teardown,
                            SuiteTermFunc init, SuiteTermFunc term);
/**
 * Register function to testsuite
 * 
 * @param suite TestSuite object to register testcase to
 * @param test TestFunc object to register
 * @param file_deps newline delimited string of all files this case depends on
 * @param test_name name to call this test case
 * @param args argument object for given case
 */
int testsuite_reg_case(TestSuite *suite, TestFunc test, Dependency *file_deps,
                       char *test_name, void *args);
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


// TestRunner object def
/**
 * test runner "object"
 */
struct testrunner_t {
    TestSuite **suites;    /**< dynamic array of TestSuite objects */
    int arr_size;          /**< size of dynamic buffers in number of elements */
    int suite_count;       /**< number of TestSuite objects in runner */
    int current_suite;     /**< index of current suite in array */
};
// TestRunner object manipulators
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
void testrunner_reg_suite(TestRunner *runner, TestSuite **suite);
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

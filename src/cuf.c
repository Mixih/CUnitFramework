/** 
 * @file cuf.c
 * @brief Custom CUnitFramework (CUF): Primary Implementations
 * @details Implementations of the primary functions defined in cuf.h. Lots of
 * dynamic memory, and other questionables here...
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cuf.h"
#include "cuf_util.h"

TestCase *testcase_create(TestFunc funct, char* test_name, Dependency *deps, void *args) {
    TestCase *testcase = (TestCase *) malloc(sizeof(TestCase));
    testcase->testfunc = funct;
    testcase->status = 0;
    testcase->err_msgs = NULL;
    testcase->err_msg_count = 0;
    testcase->err_size = 0;
    testcase->args  = args;
    testcase->test_name = malloc(sizeof(char) * (strlen(test_name)+1));
    strcpy(testcase->test_name, test_name);
    testcase->deps = deps;
    return testcase;
}

void testcase_destroy(TestCase *testcase) {
    for(int i = 0; i < testcase->err_msg_count; ++i) {
        free(testcase->err_msgs[i]);
    }
    if(testcase->err_msgs) free(testcase->err_msgs);
    if(testcase->test_name) free(testcase->test_name);
    free(testcase);
}

TestSuite *testsuite_create(char* name, SetupFunc setup, TeardownFunc teardown,
                            SuiteTermFunc init, SuiteTermFunc term) {
    TestSuite *suite = malloc(sizeof(TestSuite));
    suite->testcases = (TestCase **) malloc(sizeof(TestCase*) * 8);
    suite->setup = setup;
    suite->teardown = teardown;
    suite->init = init;
    suite->term = term;
    // allocate dynamic buffer for name
    suite->name = malloc(sizeof(char) * (strlen(name)+1));
    strcpy(suite->name, name);
    suite->arr_size = 8;
    suite->test_count = 0;
    suite->current_test = 0;
    suite->passed = 0;
    suite->failed = 0;
    suite->skipped = 0;
    return suite;
}

int testsuite_reg_case(TestSuite *suite, TestFunc test, Dependency *deps,
                       char *test_name, void *args) {
    // create from params and specified arguments and enter it into the suite's
    // testcase tracker
    suite->testcases[suite->test_count] = testcase_create(test,
                                                          test_name,
                                                          deps,
                                                          args);
    ++(suite->test_count);

    // realloc dynamic buffers if we hit the end
    if(suite->test_count == suite->arr_size) {
        suite->arr_size *= 2;
        suite->testcases = 
            (TestCase **) realloc(suite->testcases, 
                                  sizeof(TestCase*) * suite->arr_size);
    }
    return 0;
}
int testsuite_record_fail(TestSuite *suite, char* err_msg) {
    // convinience pointer
    TestCase *c_case = suite->testcases[suite->current_test];
    // reallocate buffers if there isn't one or we hit the end
    if(c_case->err_msg_count == c_case->err_size) {
        if(c_case->err_size == 0) {
            c_case->err_size = 2;
            c_case->err_msgs = 
                (char **) malloc(sizeof(char*) *  c_case->err_size);
        } else {
            c_case->err_size *= 2;
            c_case->err_msgs = 
                (char **) realloc(c_case->err_msgs,
                                  sizeof(char*) * c_case->err_size);
        }
    }
    // create dynamic error message buffer
    c_case->err_msgs[c_case->err_msg_count] = 
        malloc(sizeof(char) * (strlen(err_msg)+1));
    strcpy(c_case->err_msgs[c_case->err_msg_count], err_msg);
    ++(c_case->err_msg_count);
    c_case->status = CUF_TC_FAIL;
    return 0;
}

int testsuite_run(TestSuite *suite) {
    // run init func
    if(suite->init) suite->init(suite);
    // convenience pointer
    int *ctest = &(suite->current_test);
    // iterate over all testcases and run them, recording results
    for(*ctest = 0; *ctest < suite->test_count; ++(*ctest)) {
        TestCase *c_case = suite->testcases[*ctest];
        bool deps_ok = dependency_check(c_case->deps);
        if(deps_ok){
            void *uut = NULL;
            if(suite->setup) {
                suite->setup(&uut,
                             (suite->testcases[*ctest]->args),
                             c_case);
            }
            suite->testcases[*ctest]->testfunc(uut, suite);
            if(suite->teardown) {
                suite->teardown(uut,
                                (suite->testcases[*ctest]->args),
                                c_case);
            }
        } else {
            c_case->status = -2;
        }
        switch(c_case->status) {
            case CUF_TC_PASS:
                ++(suite->passed);
                putchar('.');
                break;
            case CUF_TC_FAIL:
                ++(suite->failed);
                putchar('x');
                break;
            case CUF_TC_SKIP:
                ++(suite->skipped);
                putchar('s');
                break;
        }
        fflush(stdout);
    }
    // run the termination function
    if(suite->term) suite->term(suite);
    return 0;
}

int testsuite_destroy(TestSuite *suite) {
    // recursive call into the testcases to clean them up
    for(int i = 0; i < suite->test_count; ++i) {
        testcase_destroy(suite->testcases[i]);
    }
    if(suite->testcases) free(suite->testcases);
    if(suite->name) free(suite->name);
    free(suite);
    return 0;
}

TestRunner *testrunner_create() {
    TestRunner *test = (TestRunner *) malloc(sizeof(TestRunner));
    test->suites = (TestSuite **) malloc(sizeof(TestSuite*) * 8);
    test->arr_size = 0;
    test->suite_count = 0;
    test->current_suite = 0;
    return test;
}

void testrunner_reg_suite(TestRunner *runner, TestSuite **suite) {
    runner->suites[runner->suite_count] = *suite;
    ++(runner->suite_count);

    // realloc dynamic buffers if we hit the end
    if(runner->suite_count == runner->arr_size) {
        runner->arr_size *= 2;
        runner->suites = 
            (TestSuite **) realloc(runner->suites, 
                                   sizeof(TestSuite*) * runner->arr_size);
    }
}

int testrunner_run(TestRunner *runner) {
    int total_tests = 0;
    int total_failed = 0;
    int total_passed = 0;
    int total_skipped = 0;
    int longest_suite_name = 0;
    // determine correct text alignment offsets
    for(int i = 0; i < runner->suite_count; ++i) {
        int length = strlen(runner->suites[i]->name)
                     + runner->suites[i]->test_count;
        total_tests += runner->suites[i]->test_count;
        if(length > longest_suite_name) { 
            longest_suite_name = length;
        }
    }
    printf("\nRegistered %d tests. Starting testing.\n", total_tests);

    // run each suite, sequentially
    int *csuite = &(runner->current_suite);
    printf("\n--------Test Progress:---------\n\n");
    for(*csuite = 0; *csuite < runner->suite_count; ++(*csuite)) {
        TestSuite *suite = runner->suites[*csuite];
        printf("Test Suite: %s ", runner->suites[*csuite]->name);
        // run currnet suite
        testsuite_run(suite);
        // right align pass/fail messages
        int name_diff = longest_suite_name -
                        (strlen(runner->suites[*csuite]->name) +
                         runner->suites[*csuite]->test_count);
        while(name_diff) {
            fputc(' ', stdout);
            --name_diff;
        }
        // print pass/fail
        if(suite->failed > 0) {
            printf("    FAILED\n");
        } else {
            printf("    PASSED\n");
        }
        // update running counters
        total_passed += suite->passed;
        total_failed += suite->failed;
        total_skipped += suite->skipped;
    }
    // remove csuite ref so we don't accidentally use it later
    csuite = NULL;
    // print failures
    bool first_fail = true;
    for(int i = 0; i < runner->suite_count; ++i) {
        TestSuite *suite = runner->suites[i];
        if(suite->failed > 0) {
            if(first_fail) {
                printf("\n-----------FAILURES:-----------\n");
                first_fail = false;
            }
            for(int j = 0; j < suite->test_count; ++j) {
                for(int k = 0; k < suite->testcases[j]->err_msg_count; ++k) {
                    printf("\n%s\n", suite->testcases[j]->err_msgs[k]);
                }
            }
        }
    }
    // print skipped tests
    bool first_skip = true;
    for(int i = 0; i < runner->suite_count; ++i) {
        TestSuite *suite = runner->suites[i];
        if(suite->skipped > 0) {
            if(first_skip) {
                printf("\n-----------SKIPPED TESTS:-----------\n");
                first_skip = false;
            }
            for(int j = 0; j < suite->test_count; ++j) {
                if(suite->testcases[j]->status == -2) {
                    printf("\nIn suite: %s, skipped testcase: %s due to "
                           "missing test files", suite->name,
                           suite->testcases[j]->test_name);
                }
            }
            printf("\n");
        }
    }
    printf("\n------------RESULTS:------------\n");
    printf("\n%d Tests completed, %d passed, %d skipped, %d failed\n\n", 
           total_tests, total_passed, total_skipped, total_failed);
    if(total_failed == 0) {
        return 0;
    } else {
        return 1;
    }
}

void testrunner_destroy(TestRunner *runner) {
    // recursive call into suites to destroy them all
    for(int i = 0; i < runner->suite_count; ++i) {
        testsuite_destroy(runner->suites[i]);
    }
    if(runner->suites) free(runner->suites);
    free(runner);
}

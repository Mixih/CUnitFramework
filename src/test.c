#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "test_util.h"

TestCase *testcase_create(TestFunc funct, char* test_name, char *file_deps, Args *args) {
    TestCase *testcase = (TestCase *) malloc(sizeof(TestCase));
    testcase->testfunc = funct;
    testcase->status = 0;
    testcase->err_msgs = NULL;
    testcase->err_msg_count = 0;
    testcase->err_size = 0;
    memcpy(&(testcase->args), args, sizeof(Args));
    testcase->test_name = malloc(sizeof(char) * (strlen(test_name)+1));
    strcpy(testcase->test_name, test_name);
    if(strlen(file_deps) != 0) {
        testcase->file_deps = malloc(sizeof(char) * (strlen(file_deps)+1));
        strcpy(testcase->file_deps, file_deps);
    } else {
        testcase->file_deps = NULL;
    }
    return testcase;
}

void testcase_destroy(TestCase *testcase) {
    for(int i = 0; i < testcase->err_msg_count; ++i) {
        free(testcase->err_msgs[i]);
    }
    if(testcase->err_msgs) free(testcase->err_msgs);
    if(testcase->test_name) free(testcase->test_name);
    if(testcase->file_deps) free(testcase->file_deps);
    free(testcase);
}

TestSuite *testsuite_create(SetupFunc setup, TeardownFunc teardown) {
    TestSuite *suite = malloc(sizeof(TestSuite));
    suite->testcases = (TestCase **) malloc(sizeof(TestCase*) * 8);
    suite->setup = setup;
    suite->teardown = teardown;
    suite->arr_size = 8;
    suite->test_count = 0;
    suite->current_test = 0;
    suite->passed = 0;
    suite->failed = 0;
    suite->skipped = 0;
    return suite;
}

int testsuite_reg_case(TestSuite *suite, TestFunc test, char *file_deps,
                       char *test_name, Args *args) {
    suite->testcases[suite->test_count] = testcase_create(test, test_name, file_deps, args);
    ++(suite->test_count);
    if(suite->test_count == suite->arr_size) {
        suite->arr_size *= 2;
        suite->testcases = (TestCase **) realloc(suite->testcases, 
                                                sizeof(TestCase*) * suite->arr_size);
    }
    return 0;
}
int testsuite_record_fail(TestSuite *suite, char* err_msg) {
    TestCase *c_case = suite->testcases[suite->current_test];
    if(c_case->err_msg_count == c_case->err_size) {
        if(c_case->err_size == 0) {
            c_case->err_size = 2;
            c_case->err_msgs = (char **) malloc(sizeof(char*) *  c_case->err_size);
        } else {
            c_case->err_size *= 2;
            c_case->err_msgs = (char **) realloc(c_case->err_msgs, sizeof(char*) *  c_case->err_size);
        }
    }
    c_case->err_msgs[c_case->err_msg_count] = malloc(sizeof(char) * (strlen(err_msg)+1));
    strcpy(c_case->err_msgs[c_case->err_msg_count], err_msg);
    ++(c_case->err_msg_count);
    c_case->status = -1;
    return 0;
}

int testsuite_run(TestSuite *suite) {
    int *ctest = &(suite->current_test);
    for(*ctest = 0; *ctest < suite->test_count; ++(*ctest)) {
        TestCase *c_case = suite->testcases[*ctest];
        bool deps_ok = (c_case->file_deps)? util_check_dep(c_case->file_deps) : true;
        if(deps_ok){
            DEFINE_UUT(uut);
            suite->setup(&uut, &(suite->testcases[*ctest]->args));
            suite->testcases[*ctest]->testfunc(&uut, suite);
            suite->teardown(&uut, &(suite->testcases[*ctest]->args));
        } else {
            c_case->status = -2;
        }
        switch(c_case->status) {
            case 0:
                ++(suite->passed);
                putchar('.');
                break;
            case -1:
                ++(suite->failed);
                putchar('x');
                break;
            case -2:
                ++(suite->skipped);
                putchar('s');
                break;
        }
        fflush(stdout);
    }
    return 0;
}

int testsuite_destroy(TestSuite *suite) {
    for(int i = 0; i < suite->test_count; ++i) {
        testcase_destroy(suite->testcases[i]);
    }
    if(suite->testcases) free(suite->testcases);
    free(suite);
    return 0;
}

TestRunner *testrunner_create() {
    TestRunner *test = (TestRunner *) malloc(sizeof(TestRunner));
    test->suites = (TestSuite **) malloc(sizeof(TestSuite*) * 8);
    test->suite_names = (char **) malloc(sizeof(char*)*8);
    test->arr_size = 0;
    test->suite_count = 0;
    test->current_suite = 0;
    return test;
}

void testrunner_reg_suite(TestRunner *runner, TestSuite **suite, char *name) {
    runner->suites[runner->suite_count] = *suite;
    runner->suite_names[runner->suite_count] = malloc(sizeof(char) * (strlen(name)+1));
    strcpy(runner->suite_names[runner->suite_count], name);
    ++(runner->suite_count);
    if(runner->suite_count == runner->arr_size) {
        runner->arr_size *= 2;
        runner->suites = (TestSuite **) realloc(runner->suites, sizeof(TestSuite*) * runner->arr_size);
        runner->suite_names = (char **) realloc(runner->suite_names, sizeof(char*) * runner->arr_size);
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
        int length = strlen(runner->suite_names[i]) + runner->suites[i]->test_count;
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
        printf("Test Suite: %s ", runner->suite_names[*csuite]);
        // run currnet suite
        testsuite_run(suite);
        // right align pass/fail messages
        int name_diff = longest_suite_name -
                        (strlen(runner->suite_names[*csuite]) +
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
                           "missing test files", runner->suite_names[i],
                           suite->testcases[j]->test_name);
                }
            }
            printf("\n");
        }
    }
    printf("\n------------RESULTS:------------\n");
    printf("\n%d Tests completed, %d passed, %d skipped, %d failed\n\n", 
           total_tests, total_passed, total_skipped, total_failed);
    if(total_skipped == 0 && total_failed == 0) {
        return 0;
    } else {
        return 1;
    }
}

void testrunner_destroy(TestRunner *runner) {
    for(int i = 0; i < runner->suite_count; ++i) {
        testsuite_destroy(runner->suites[i]);
    }
    for(int i = 0; i < runner->suite_count; ++i) {
        free(runner->suite_names[i]);
    }
    if(runner->suites) free(runner->suites);
    if(runner->suite_names) free(runner->suite_names);
    free(runner);
}

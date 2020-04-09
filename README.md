# CUnitFramework (CUF)

A unit testing library written in pure C because I shouldn't have.

## Usage

1. Define a "uut" object 
2. Declare setup and teardown functions using the SETUPFUNC and TEARDOWNFUNC
   macros respectively. Note that the name you provide as the argument will be
   the name of the function
3. Define testcases using the `TESTCASE(<name>)` macro.
4. Define a testsuite that hold your testcases along with setup and teardown
   function
5. Register testcases to the testsuite
6. Define a test runner that handles runnign testcases and reporting
7. Register suite to test runner
8. Put three lines (minimal) into `main()`, and run.

## Example
```C
#include "test.h"

SETUPFUNC(setup) {
    // common setup commands to run
}

TEARDOWNFUNC(teardown) {
    // common teardown commands to run (memory deallocation, etc)
}

TESTCASE(tc1) {
    // first test case. will be passed a `uut` object and a `TestSuite` object
    // asserts that the two arguments are equal, and trascks the results into
    // the testcase object
    ASSERT_EQUAL(1,1);
}

TestRunner *create_tests() {
    TestRunner *runner = testrunner_create();
    Args args = {true, "ifile.temp", "ofile.temp"};

    TestSuite *suite_1 = testsuite_create(&setup, &teardown);
    // register the suite we defined above with a dependency on file_dep.txt
    REGISTER_TESTCASE(suite_1, &tc1, "file_dep.txt", &args);
    testrunner_reg_suite(runner, &suite_1, "Test Suite Name");

    return runner;
}

int main() {
    TestRunner *test = create_tests();
    testrunner_run(test);
    testrunner_destroy(test);
}
```

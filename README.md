# MINTEST

Mintest is a minimal unit testing framework for C self-contained in a single header file.

It provides a way to define and configure test suites and a few handy assertion types. It reports the summary of the number of tests run, number of assertions and time elapsed.

Mintest runs every test in a subprocess, so you don't have to worry about a segfault in one of your tests crashing your whlole test suite :wink:

This project was inspired by [Minunit](https://github.com/siu/minunit/tree/master)

## How to use it

This is a minimal test suite written with mintest:

```c
#include "mintest.h"

MINTEST_CASE(test_assert) {
	mintest_assert(5 == 7);
}

MINTEST_SUITE(test_suite) {
	MINTEST_RUN_TEST_CASE(test_assert);
}

int main(void) {
    MINTEST_SETUP();
    MINTEST_RUN_TEST_SUITE(test_suite);
    MINTEST_REPORT();
    MINTEST_CLEANUP();
    return 0;
};
```

Which will produce the following output:

```
5 == 7 assertion failed:
        mintest_usage.c:51
✘ {test_suite:test_check} failed after 0.001s [1]


TEST RUN FINISHED 🫠
Total tests - 1 ,Passed - 0, Failed - 1, Assertions - 1
Finished in 0.887s
```

Check out `mintest_usage.c` to see a complete example. 

Compile with 

```
gcc -o mintest_usage.o mintest_usage.c
```

## Setup and teardown functions

One can define setup and teardown functions that run before and after every test respectfully and configure the test suite to run
them by using the macro `MINTEST_SETUP_TEST_SUITE` from within a `MINTEST_TEST_SUITE`
declaration.

## Assertion types

`mintest_assert(condition)`: will pass if the condition is evaluated to `true`, otherwise
it will show the condition as the error message

`mintest_assert_false(condition)`: will pass if the condition is evaluated to `false`, otherwise
it will show the condition as the error message

`mintest_fail(message)`: will fail and show the message

`mintest_assert_int_eq(expected, result)`: will pass if the two numbers are
equal or show their values as the error message

`mintest_assert_double_eq(expected, result)`: will pass if the two values
are almost equal or show their values as the error message. The value of
`MINTEST_EPSILON` sets the threshold to determine if the values are close enough.

`mintest_assert_string_eq(expected, resurt):` will pass if the two strings are equal.

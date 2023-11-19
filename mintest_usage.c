#include "mintest.h"

int foo, bar;
const char *baz = "baz";

void test_setup(void) {
    foo = 5;
    bar = 7;
}

void test_teardown(void) {
    /* Do nothing */
}

MINTEST_CASE(segfault) {
    for (int i = 0; i < 100050000; ++i) {};
    *(volatile int*)NULL;
};

MINTEST_CASE(assert_succeeds) {
    mintest_assert(1 == 1);
}

MINTEST_CASE(assert_fails) {
    mintest_assert(1 == 2);
}

MINTEST_CASE(fail) {
    mintest_fail("Just failing");
}

MINTEST_CASE(assert_false_succeeds) {
    mintest_assert_false(1 != 1);
}

MINTEST_CASE(assert_double_eq_succeeds) {
    mintest_assert_double_eq(1.0, 1.0);
}

MINTEST_CASE(assert_double_eq_fails) {
    mintest_assert_double_eq(5.7, 1.2);
}

MINTEST_CASE(assert_string_eq_succeeds) {
    mintest_assert_string_eq(baz, "baz");
}

MINTEST_CASE(assert_string_eq_fails) {
    mintest_assert_string_eq(baz, "foo");
}


MINTEST_SUITE(test_suite) {
    MINTEST_SETUP_TEST_SUITE(test_setup, test_teardown);
    MINTEST_RUN_TEST_CASE(segfault);
    MINTEST_RUN_TEST_CASE(assert_succeeds);
    MINTEST_RUN_TEST_CASE(assert_fails);
    MINTEST_RUN_TEST_CASE(fail);
    MINTEST_RUN_TEST_CASE(assert_false_succeeds);
    MINTEST_RUN_TEST_CASE(assert_double_eq_succeeds);
    MINTEST_RUN_TEST_CASE(assert_double_eq_fails);
    MINTEST_RUN_TEST_CASE(assert_string_eq_succeeds);
    MINTEST_RUN_TEST_CASE(assert_string_eq_fails);
};

int main(void) {
    MINTEST_SETUP();
    MINTEST_RUN_TEST_SUITE(test_suite);
    MINTEST_REPORT();
    MINTEST_CLEANUP();
    return 0;
};
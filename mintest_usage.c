#include "mintest.h"

MINTEST_CASE(segfault_test) {
    for (int i = 0; i < 100050000; ++i) {};
    *(volatile int*)NULL;
};

MINTEST_CASE(assert_succeeds_test) {
    mintest_assert(1 == 1);
}

MINTEST_CASE(assert_fails_test) {
    mintest_assert(1 == 2);
}

MINTEST_CASE(fail_test) {
    mintest_fail("Just failing");
}

MINTEST_CASE(assert_false_succeeds_test) {
    mintest_assert_false(1 != 1);
}

void test_setup(void) {
    int b = 7;
}

void test_teardown(void) {
    int a = 5;
}

MINTEST_SUITE(test_suite) {
    MINTEST_SETUP_TEST_SUITE(test_setup, test_teardown);
    MINTEST_RUN_TEST_CASE(segfault_test);
    MINTEST_RUN_TEST_CASE(assert_succeeds_test);
    MINTEST_RUN_TEST_CASE(assert_fails_test);
    MINTEST_RUN_TEST_CASE(fail_test);
    MINTEST_RUN_TEST_CASE(assert_false_succeeds_test);
};

int main(void) {
    MINTEST_SETUP();
    MINTEST_RUN_TEST_SUITE(test_suite);
    MINTEST_REPORT();
    MINTEST_CLEANUP();
    return 0;
};
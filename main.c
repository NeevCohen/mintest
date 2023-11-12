#include "mintest.h"

TEST_CASE(failing_test) {
    printf("In failing test\n");
    for (int i = 0; i < 100000000; ++i) {};
    *(volatile int*)NULL;
};

TEST_CASE(passing_test) {
    printf("In passing test\n");
    for (int i = 0; i < 100000000; ++i) {};
}

TEST_SUITE(test_suite) {
    RUN_TEST_CASE(failing_test);
    RUN_TEST_CASE(passing_test);
};

int main(void) {
    RUN_TEST_SUITE(test_suite);
    return 0;
};
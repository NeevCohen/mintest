// Author: Neev Cohen - https://github.com/NeevCohen
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20 /* Don't use a file.  */
#endif

/*  Accuracy with which floats are compared */
#define MINTEST_EPSILON 1E-12

const char *melting_emoji = "\U0001FAE0";
const char *party_emoji = "\U0001F973";

size_t mintest_num_tests = 0, mintest_num_fails = 0, mintest_num_passes = 0, *mintest_num_assertions = NULL;
double mintest_seconds_spent = 0;

void (*mintest_test_setup)(void);
void (*mintest_test_teardown)(void);

static inline void red(void)
{
    printf("\033[1;31m");
}

static inline void green(void)
{
    printf("\033[1;32m");
}

static inline void reset(void)
{
    printf("\033[0m");
}

#define __MINTEST_SAFE_BLOCK(block) \
    do                              \
    {                               \
        block                       \
    } while (0)

#define MINTEST_CASE(test) static void test(void)

#define MINTEST_SUITE(suite) static void suite(void)

#define __MINTEST_RUN_TEST_CASE(test, caller) __MINTEST_SAFE_BLOCK(                                                          \
    pid_t pid;                                                                                                               \
    int status;                                                                                                              \
    int exit_status;                                                                                                         \
    struct timeval begin;                                                                                                    \
    struct timeval end;                                                                                                      \
    int microseconds_spent;                                                                                                  \
    double seconds_spent;                                                                                                    \
                                                                                                                             \
    if (mintest_test_setup)                                                                                                  \
        mintest_test_setup();                                                                                                \
    pid = fork();                                                                                                            \
    switch (pid) {                                                                                                           \
        case -1:                                                                                                             \
            perror("fork");                                                                                                  \
            break;                                                                                                           \
        case 0:                                                                                                              \
            test();                                                                                                          \
            exit(EXIT_SUCCESS);                                                                                              \
        default:                                                                                                             \
            gettimeofday(&begin, NULL);                                                                                      \
            waitpid(pid, &status, 0);                                                                                        \
            mintest_num_tests++;                                                                                             \
            if (mintest_test_teardown)                                                                                       \
                mintest_test_teardown();                                                                                     \
            gettimeofday(&end, NULL);                                                                                        \
            microseconds_spent = ((end.tv_sec - begin.tv_sec) * 1000000) + (end.tv_usec - begin.tv_usec);                    \
            seconds_spent = (double)microseconds_spent / 1000000;                                                            \
            mintest_seconds_spent += seconds_spent;                                                                          \
            if (WIFEXITED(status))                                                                                           \
            {                                                                                                                \
                exit_status = WEXITSTATUS(status);                                                                           \
                if (exit_status == 0)                                                                                        \
                {                                                                                                            \
                    mintest_num_passes++;                                                                                    \
                    green();                                                                                                 \
                    printf("\u2714 {%s:%s} passed after %.3fs [%d]\n\n", caller, #test, seconds_spent, WEXITSTATUS(status)); \
                    reset();                                                                                                 \
                }                                                                                                            \
                else                                                                                                         \
                {                                                                                                            \
                    mintest_num_fails++;                                                                                     \
                    red();                                                                                                   \
                    printf("\u2718 {%s:%s} failed after %.3fs [%d]\n\n", caller, #test, seconds_spent, WEXITSTATUS(status)); \
                    reset();                                                                                                 \
                }                                                                                                            \
            }                                                                                                                \
            else                                                                                                             \
            {                                                                                                                \
                mintest_num_fails++;                                                                                         \
                red();                                                                                                       \
                printf("\u2718 {%s:%s} failed after %.3fs [%d]\n\n", caller, #test, seconds_spent, WTERMSIG(status));        \
                reset();                                                                                                     \
            }                                                                                                                \
            break;                                                                                                           \
    })

#define MINTEST_RUN_TEST_CASE(test) __MINTEST_RUN_TEST_CASE(test, __func__)

#define MINTEST_RUN_TEST_SUITE(suite) suite()

#define MINTEST_SETUP_TEST_SUITE(setup, teardown) __MINTEST_SAFE_BLOCK( \
    mintest_test_setup = setup;                                         \
    mintest_test_teardown = teardown;)

#define MINTEST_SETUP() __MINTEST_SAFE_BLOCK(                                                                       \
    mintest_num_assertions = mmap(NULL, sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); \
    if (mintest_num_assertions == NULL) {                                                                           \
        printf("Failed to allocate memory for mintest_num_assertions\n");                                           \
        exit(EXIT_FAILURE);                                                                                         \
    };)

#define MINTEST_CLEANUP() __MINTEST_SAFE_BLOCK( \
    if (mintest_num_assertions)                 \
        munmap(mintest_num_assertions, sizeof(*mintest_num_assertions));)

#define MINTEST_REPORT() __MINTEST_SAFE_BLOCK(                                                                                                                                                         \
    printf("\n\nTEST RUN FINISHED %s\n", mintest_num_fails > 0 ? melting_emoji : party_emoji);                                                                                                         \
    printf("Total %s - %u ,Passed - %u, Failed - %u, Assertions - %u\n", mintest_num_tests > 1 ? "tests" : "test", mintest_num_tests, mintest_num_passes, mintest_num_fails, *mintest_num_assertions); \
    printf("Finished in %.3fs\n", mintest_seconds_spent);)

/*
Assertions
*/

#define mintest_fail(message) __MINTEST_SAFE_BLOCK(                         \
    if (mintest_num_assertions)(*mintest_num_assertions)++;                 \
    printf("assertion failed:\n\t%s:%d %s\n", __FILE__, __LINE__, message); \
    exit(EXIT_FAILURE);)

#define mintest_assert(test) __MINTEST_SAFE_BLOCK(                            \
    if (mintest_num_assertions)(*mintest_num_assertions)++;                   \
    if (!(test)) {                                                            \
        printf("%s assertion failed:\n\t%s:%d\n", #test, __FILE__, __LINE__); \
        exit(EXIT_FAILURE);                                                   \
    } else {                                                                  \
        printf(".");                                                          \
    })

#define mintest_assert_false(test) mintest_assert(!(test))

#define mintest_assert_int_eq(expected, result) __MINTEST_SAFE_BLOCK(                                                     \
    int mintest_tmp_e;                                                                                                    \
    int mintest_tmp_r;                                                                                                    \
    if (mintest_num_assertions)(*mintest_num_assertions)++;                                                               \
    mintest_tmp_e = (expected);                                                                                           \
    mintest_tmp_r = (result);                                                                                             \
    if (mintest_tmp_e != mintest_tmp_r) {                                                                                 \
        printf("assertion failed:\n\t%s:%d: %d expected but was %d\n", __FILE__, __LINE__, mintest_tmp_e, mintest_tmp_r); \
        exit(EXIT_FAILURE);                                                                                               \
    } else {                                                                                                              \
        printf(".");                                                                                                      \
    })

#define mintest_assert_double_eq(expected, result) __MINTEST_SAFE_BLOCK(                                                                                                                \
    double mintest_tmp_e;                                                                                                                                                               \
    double mintest_tmp_r;                                                                                                                                                               \
    if (mintest_num_assertions)(*mintest_num_assertions)++;                                                                                                                             \
    mintest_tmp_e = (expected);                                                                                                                                                         \
    mintest_tmp_r = (result);                                                                                                                                                           \
    if (fabs(mintest_tmp_e - mintest_tmp_r) > MINTEST_EPSILON) {                                                                                                                        \
        int mintest_significant_figures = 1 - log10(MINTEST_EPSILON);                                                                                                                   \
        printf("assertion failed:\n\t%s:%d: %.*g expected but was %.*g\n", __FILE__, __LINE__, mintest_significant_figures, mintest_tmp_e, mintest_significant_figures, mintest_tmp_r); \
        exit(EXIT_FAILURE);                                                                                                                                                             \
    } else {                                                                                                                                                                            \
        printf(".");                                                                                                                                                                    \
    })

#define mintest_assert_string_eq(expected, result) __MINTEST_SAFE_BLOCK(                                                      \
    const char *mintest_tmp_e = expected;                                                                                     \
    const char *mintest_tmp_r = result;                                                                                       \
    if (mintest_num_assertions)(*mintest_num_assertions)++;                                                                   \
    if (!mintest_tmp_e) {                                                                                                     \
        mintest_tmp_e = "<null pointer>";                                                                                     \
    } if (!mintest_tmp_r) {                                                                                                   \
        mintest_tmp_r = "<null pointer>";                                                                                     \
    } if (strcmp(mintest_tmp_e, mintest_tmp_r) != 0) {                                                                        \
        printf("assertion failed:\n\t%s:%d: '%s' expected but was '%s'\n", __FILE__, __LINE__, mintest_tmp_e, mintest_tmp_r); \
        exit(EXIT_FAILURE);                                                                                                   \
    } else {                                                                                                                  \
        printf(".");                                                                                                          \
    })

// Author: Neev Cohen - https://github.com/RandomSpaceMonkey
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>

static inline void red (void) {
  printf("\033[1;31m");
}

static inline void green(void) {
  printf("\033[1;32m");
}

static inline void reset(void) {
  printf("\033[0m");
}


#define __MINTEST_SAFE_BLOCK(block) \
    do                              \
    {                               \
        block                       \
    } while (0)

#define TEST_CASE(test) static void test(void)

#define TEST_SUITE(suite) static void suite(void)

#define __RUN_TEST_CASE(test, caller) __MINTEST_SAFE_BLOCK(                                               \
    pid_t pid;                                                                                            \
    int status;                                                                                           \
    struct timeval begin;                                                                                 \
    struct timeval end;                                                                                   \
    int microseconds_spent;                                                                                       \
    double seconds_spent; \
                                                                                                          \
    pid = fork();                                                                                         \
    switch (pid) {                                                                                        \
        case -1:                                                                                          \
            perror("fork");                                                                               \
            break;                                                                                        \
        case 0:                                                                                           \
            test();                                                                                       \
            break;                                                                                        \
        default:                                                                                          \
            gettimeofday(&begin, NULL);                                                                   \
            waitpid(pid, &status, 0);                                                                     \
            gettimeofday(&end, NULL);                                                                     \
            microseconds_spent = ((end.tv_sec - begin.tv_sec) * 1000000) + (end.tv_usec - begin.tv_usec);         \
            seconds_spent = (double)microseconds_spent / 1000000 ; \
            if (WIFEXITED(status))                                                                        \
            {                                                                                             \
                green(); \
                printf("\u2714 {%s:%s} passed after %.3fs [%d]\n", caller, #test, seconds_spent, WEXITSTATUS(status)); \
                reset(); \
            }                                                                                             \
            else if (WIFSIGNALED(status))                                                                 \
            {                                                                                             \
                red(); \
                printf("\u2718 {%s:%s} failed after %.3fs [%d]\n", caller, #test, seconds_spent, WTERMSIG(status));    \
                reset(); \
            }                                                                                             \
            break;                                                                                        \
    })

#define RUN_TEST_CASE(test) __RUN_TEST_CASE(test, __func__)

#define RUN_TEST_SUITE(suite) suite()

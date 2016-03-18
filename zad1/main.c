#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

int licznik = 0;

int fn(void *arg) {
    licznik++;
    _exit(0);
}

int main(int argc, char *argv[]) {

    pid_t pid;
    if (argc < 3) {
        printf("Not enough arguments");
        exit(1);
    }
    int n = atoi(argv[1]);
    if (n == 0) {
        printf("Wrong first argument, should be a natural number");
        exit(1);
    }
    clock_t starttime = clock();
    char *op = argv[2];
    char option = op[0];
    void *child_stack = malloc(8192);
    clock_t childtimes = 0;
    child_stack += 8192;
    for (int i = 0; i < n; i++) {
        switch (option) {
            case 'a': {
                pid = fork();
                if (pid == 0) {
                    clock_t t = clock();
                    licznik++;
                    t = clock() - t;
                    _exit(t);
                } else {
                    int status;
                    wait(&status);
                    childtimes += WEXITSTATUS(status);
                }
                break;
            }
            case 'b': {
                pid = vfork();
                if (pid == 0) {
                    clock_t t = clock();
                    licznik++;
                    t = clock() - t;
                    _exit(t);
                } else {
                    int status;
                    wait(&status);
                    childtimes += WEXITSTATUS(status);
                }
                break;
            }
            case 'c': {
                clone(&fn, child_stack, SIGCHLD, NULL);
                int status;
                wait(&status);
                childtimes += WEXITSTATUS(status);
                break;
            }
            case 'd': {
                clone(&fn, child_stack, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
                int status;
                wait(&status);
                childtimes += WEXITSTATUS(status);
            }
                break;
            default:
                break;
        }
    }
    clock_t endtime = clock() - starttime;
    struct rusage *rus = malloc(sizeof(struct rusage));
    getrusage(RUSAGE_SELF, rus);
    printf("System Time: %li, User Time: %li, User+System Time: %li, Real Time: %li\n",
           rus->ru_stime.tv_sec, rus->ru_utime.tv_sec, rus->ru_utime.tv_sec + rus->ru_stime.tv_sec,
           endtime / CLOCKS_PER_SEC);
    printf("\n");
    struct rusage *rus1 = malloc(sizeof(struct rusage));
    getrusage(RUSAGE_CHILDREN, rus1);
    printf("System Time: %li, User Time: %li, User+System Time: %li, Real Time: %li\n",
           rus1->ru_stime.tv_sec, rus1->ru_utime.tv_sec,
           rus1->ru_utime.tv_sec + rus1->ru_stime.tv_sec, childtimes / CLOCKS_PER_SEC);
    printf("\n");

    printf("System Time: %li, User Time: %li, User+System Time: %li, Real Time: %li\n",
           rus1->ru_stime.tv_sec + rus->ru_stime.tv_sec, rus->ru_utime.tv_sec + rus1->ru_utime.tv_sec,
           rus->ru_utime.tv_sec + rus->ru_stime.tv_sec + rus1->ru_utime.tv_sec + rus1->ru_stime.tv_sec,
           (endtime + childtimes) / CLOCKS_PER_SEC);

    return 0;
}


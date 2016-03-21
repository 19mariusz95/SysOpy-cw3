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
#include <sys/mman.h>

int licznik = 0;
clock_t childtimes = 0;
void *child_stack;


int fn(void *arg) {
    struct timespec t1, t2;
    clock_gettime(CLOCK_REALTIME, &t1);
    licznik++;
    clock_gettime(CLOCK_REALTIME, &t2);
    _exit(t2.tv_nsec - t1.tv_nsec);
}

void fun1() {
    pid_t pid = fork();
    if (pid == 0) {
        fn(NULL);
    } else {
        int status;
        wait(&status);
        childtimes += WEXITSTATUS(status);
    }
}

void fun2() {
    pid_t pid = vfork();
    if (pid == 0) {
        fn(NULL);
    } else {
        int status;
        wait(&status);
        childtimes += WEXITSTATUS(status);
    }
}

void fun3() {
    clone(&fn, child_stack, SIGCHLD, NULL);
    int status;
    wait(&status);
    childtimes += WEXITSTATUS(status);
}

void fun4() {
    clone(&fn, child_stack, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
    int status;
    wait(&status);
    childtimes += WEXITSTATUS(status);
}

int main(int argc, char *argv[]) {

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
    child_stack = mmap(0, 8192, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    child_stack += 8192;
    for (int i = 0; i < n; i++) {
        switch (option) {
            case 'a': {
                fun1();
                break;
            }
            case 'b': {
                fun2();
                break;
            }
            case 'c': {
                fun3();
                break;
            }
            case 'd': {
                fun4();
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

    printf("%s %d\n", "Counter: ", licznik);

    return 0;
}


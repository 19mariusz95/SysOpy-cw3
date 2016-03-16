#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/wait.h>
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
    char *op = argv[2];
    char option = op[0];
    clock_t t;
    struct tms *t_cpu1 = malloc(sizeof(struct tms));
    struct tms *t_cpu2 = malloc(sizeof(struct tms));
    if (t_cpu1 == NULL || t_cpu2 == NULL) {
        printf("%s", "Allocation memory failed");
        return 1;
    }
    t = times(t_cpu1);
    void *child_stack = malloc(8192);
    child_stack += 8192;
    for (int i = 0; i < n; i++) {
        switch (option) {
            case 'a': {
                pid = fork();
                if (pid == 0) {
                    licznik++;
                    _exit(0);
                } else {
                    int status;
                    wait(&status);
                }
                break;
            }
            case 'b': {
                pid = vfork();
                if (pid == 0) {
                    licznik++;
                    _exit(licznik);
                } else {
                    int status;
                    wait(&status);
                }
                break;
            }
            case 'c': {
                for (int j = 0; j < n; j++) {
                    clone(&fn, child_stack, SIGCHLD, NULL);
                    int status;
                    wait(&status);
                }
                break;
            }
            case 'd': {
                clone(&fn, child_stack, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL);
                int status;
                wait(&status);
            }
                break;
            default:
                break;
        }
    }

    t = times(t_cpu2) - t;
    printf("Real Time: %jd, User Time: %jd, System Time: %jd\n", (intmax_t) t,
           (intmax_t) (t_cpu2->tms_utime - t_cpu1->tms_utime),
           (intmax_t) (t_cpu2->tms_stime - t_cpu1->tms_stime));
    printf("licznik: %d\n", licznik);

    free(t_cpu1);
    free(t_cpu2);
    return 0;
}


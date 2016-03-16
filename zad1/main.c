#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/times.h>

int licznik = 0;

void do_stuff(int n, char option);

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
    char *op = argv[2];
    char option = op[0];

    clock_t time;
    struct tms *t_cpu1 = malloc(sizeof(struct tms));
    struct tms *t_cpu2 = malloc(sizeof(struct tms));
    if (t_cpu1 == NULL || t_cpu2 == NULL) {
        printf("%s", "Allocation memory failed");
        return 1;
    }

    time = times(t_cpu1);

    do_stuff(n, option);

    time = times(t_cpu2) - time;
    printf("Real Time: %jd, User Time: %jd, System Time: %jd\n", (intmax_t) time,
           (intmax_t) (t_cpu2->tms_utime - t_cpu1->tms_utime),
           (intmax_t) (t_cpu2->tms_stime - t_cpu1->tms_stime));


    return 0;
}

void do_stuff(int n, char option) {
    int i;
    for (i = 0; i < n; i++) {
        // TODO
    }
    printf("%d\n", licznik);
}
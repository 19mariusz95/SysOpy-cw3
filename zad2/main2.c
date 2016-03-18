#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>

int get_dirs_of_first_level(char *path, int argc, char *argv[]);

int chilren = 0;

int main(int argc, char *argv[]) {
    char *path = argv[1];
    if (path == NULL)
        path = "./";
    return get_dirs_of_first_level(path, argc, argv);
}

int get_dirs_of_first_level(char *path, int argc, char *argv[]) {
    DIR *dir = opendir(path);
    if (dir == NULL)
        return 0;
    struct stat filestat;
    stat(path, &filestat);
    if (!S_ISDIR(filestat.st_mode)) {
        closedir(dir);
        return 0;
    }
    int a = 0, b = 0;
    struct dirent *ent;
    size_t len = strlen(path);
    char fn[FILENAME_MAX];
    strcpy(fn, path);
    fn[len++] = '/';
    pid_t pid = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        strncpy(fn + len, ent->d_name, FILENAME_MAX - len);
        stat(fn, &filestat);
        a++;
        if (S_ISDIR(filestat.st_mode)) {
            pid = fork();
            if (pid == 0) {
                int tn;
                argv[1] = fn;
                tn = execv("./fcounter2", argv);
                _exit(tn);
            } else {
                chilren++;
            }
        }
    }
    closedir(dir);
    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];
        if (strcmp(arg, "-w") == 0) {
            sleep(15);
        }
    }
    for (int i = 0; i < chilren; ++i) {
        int status;
        wait(&status);
        b += WEXITSTATUS(status);
    }
    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];
        if (strcmp(arg, "-v") == 0) {
            printf("path: %s my_files: %d all_files %d\n", path, a, a + b);
            fflush(stdout);
        }
    }
    return a + b;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>

int get_dirs_of_first_level(char *path, int argc, char *argv[]);

pid_t pids[256];
int c = 0;

int main(int argc, char *argv[]) {
    char *path = getenv("PATH_TO_BROWSE");
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
    char *ext = getenv("EXT_TO_BROWSE");
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
        if (ext == NULL)
            a++;
        else {
            char *ala = strrchr(fn, '.');
            if (ala != NULL) {
                if (strcmp(ala, ext) == 0)
                    a++;
            }
        }
        if (S_ISDIR(filestat.st_mode)) {
            pid = fork();
            if (pid == 0) {
                char *e = "PATH_TO_BROWSE=";
                size_t tmp = strlen(e);
                char tmp1[FILENAME_MAX + tmp];
                strcpy(tmp1, e);
                strncpy(tmp1 + tmp, fn, FILENAME_MAX + tmp - len);
                int tn;
                if (ext != NULL) {
                    char *env[] = {tmp1, ext};
                    tn = execve("./fcounter2", argv, env);
                } else {
                    char *env[] = {tmp1};
                    tn = execve("./fcounter2", argv, env);
                }
                _exit(tn);
            } else {
                pids[c++] = pid;
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
    for (int i = 0; i < c; ++i) {
        int status;
        waitpid(pids[i], &status, 0);
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
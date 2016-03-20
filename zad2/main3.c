#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>

int get_dirs_of_first_level(char *path, char *ext, int argc, char *argv[]);

int children = 0;

int main(int argc, char *argv[]) {
    char *path = getenv("PATH_TO_BROWSE");
    char *ext = getenv("EXT_TO_BROWSE");
    if (path == NULL) {
        path = "./";
    }
    return get_dirs_of_first_level(path, ext, argc, argv);
}

int get_dirs_of_first_level(char *path, char *ext, int argc, char *argv[]) {
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
        if (ext == NULL) {
            a++;
        }
        else {
            char *ala = strrchr(fn, '.');
            if (ala != NULL && strlen(ala) > 1) {
                char *substr = calloc(strlen(ala), 1);
                strncpy(substr, ala + 1, strlen(ala) - 1);
                fflush(stdout);
                if (strcmp(substr, ext) == 0)
                    a++;
            }
        }
        if (S_ISDIR(filestat.st_mode)) {
            pid = fork();
            if (pid == 0) {
                char *e = "PATH_TO_BROWSE=";
                size_t tmp = strlen(e);
                char tmp1[FILENAME_MAX];
                strcpy(tmp1, e);
                strncpy(tmp1 + tmp, fn, FILENAME_MAX + tmp);
                int tn;
                if (ext != NULL) {
                    char *etmp = "EXT_TO_BROWSE=";
                    size_t etmpsize = strlen(etmp);
                    char *extmp = malloc((strlen(ext) + etmpsize + 1) * sizeof(char));
                    strcpy(extmp, etmp);
                    strcpy(extmp + etmpsize, ext);
                    char *env[] = {tmp1, extmp, NULL};
                    tn = execve(argv[0], argv, env);
                } else {
                    char *env[] = {tmp1, NULL};
                    tn = execve(argv[0], argv, env);
                }
                _exit(tn);
            } else {
                children++;
            }
        }
    }
    closedir(dir);
    fflush(stdout);
    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];
        if (strcmp(arg, "-w") == 0) {
            sleep(15);
        }
    }
    for (int i = 0; i < children; ++i) {
        int status = -1;
        wait(&status);
        b += WEXITSTATUS(status);
        fflush(stdout);
    }
    for (int i = 1; i < argc; ++i) {
        char *arg = argv[i];
        if (strcmp(arg, "-v") == 0) {
            printf("path: %s my_files: %d all_files %d\n", path, a, a + b);
            fflush(stdout);
        }
    }
    exit(a + b);
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <zconf.h>
#include <sys/wait.h>

void get_dirs_of_first_level(char *path, int i, int i1);

int main(int argc, char *argv[]) {
    char *path = getenv("PATH_TO_BROWSE");
    if (path == NULL)
        path = "./";
    printf("%s\n", path);
    fflush(stdout);
    get_dirs_of_first_level(path, 0, 0);
    return 0;
}

void get_dirs_of_first_level(char *path, int i, int i1) {
    DIR *dir = opendir(path);
    if (dir == NULL)
        return;
    struct stat filestat;
    stat(path, &filestat);
    if (!S_ISDIR(filestat.st_mode)) {
        closedir(dir);
        return;
    }
    int counter = 0;
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
        if (S_ISDIR(filestat.st_mode)) {
            counter++;
            pid = fork();
            if (pid == 0) {
                char *e = "PATH_TO_BROWSE=";
                size_t tmp = strlen(e);
                char tmp1[FILENAME_MAX + tmp];
                strcpy(tmp1, e);
                strncpy(tmp1 + tmp, fn, FILENAME_MAX + tmp - len);
                char *env[] = {tmp1};
                execle("./fcounter1", "", (char *) 0, env);
                _exit(0);
            }
        }
    }
    if (pid != 0)
        wait(NULL);
    closedir(dir);
    return;
}
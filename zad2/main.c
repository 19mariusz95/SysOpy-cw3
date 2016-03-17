#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

int get_dirs_of_first_level(char *env);

int main(int argc, char *argv[]) {


    char *env = getenv("PATH_TO_BROWSE");
    if (env == NULL)
        env = "./";
    int n = get_dirs_of_first_level(env);
    printf("%s %d\n", env, n);
    return 0;
}

int get_dirs_of_first_level(char *env) {
    DIR *dir = opendir(env);
    if (dir == NULL)
        return -2;
    struct stat filestat;
    stat(env, &filestat);
    if (!S_ISDIR(filestat.st_mode)) {
        closedir(dir);
        return -1;
    }
    int counter = 0;
    struct dirent *ent;
    size_t len = strlen(env);
    char fn[FILENAME_MAX];
    strcpy(fn, env);
    fn[len++] = '/';
    while ((ent = readdir(dir)) != NULL) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;
        strncpy(fn + len, ent->d_name, FILENAME_MAX - len);
        stat(fn, &filestat);
        if (S_ISDIR(filestat.st_mode))
            counter++;
    }
    closedir(dir);
    return counter;
}
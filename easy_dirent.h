#ifndef EASYDIR_H_
#define EASYDIR_H_
#include <stdbool.h>
#include <stddef.h>
#include <dirent.h>

#ifndef DT_DIR
#define DT_DIR 4
#endif // DT_DIR

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif // PATH_MAX

#ifndef EASYDIR_STRDUP
#include <stdlib.h>
char *strdup(const char *s); // for some reason LSP is blind
#define EASYDIR_STRDUP strdup
#endif // EASYDIR_STRDUP

#ifndef EASYDIR_FREE
#include <stdlib.h>
#define EASYDIR_FREE free
#endif // EASYDIR_FREE

typedef struct {
    char* path;
    DIR* dir;
}EasyDir;

typedef struct {
    const char path[PATH_MAX];
    struct dirent* entry;
}EasyDirent;

#define DIR_STACK_SIZE 1024
typedef struct {
    EasyDir dirs[DIR_STACK_SIZE];
    size_t dirs_count;
}EasyDirHandle;

bool easydir_open(EasyDirHandle* easydir, const char* path);
bool easydir_read(EasyDirHandle* self, EasyDirent* easydirent);

#endif // EASYDIR_H_

#ifdef EASYDIR_IMPLEMENTATION
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

bool easydir_open(EasyDirHandle* easydir, const char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", path, strerror(errno));
        return false;
    }

    assert(easydir->dirs_count < DIR_STACK_SIZE);
    easydir->dirs[easydir->dirs_count].dir = dir;
    easydir->dirs[easydir->dirs_count].path = EASYDIR_STRDUP(path);
    return true;
}

bool easydir_read(EasyDirHandle* self, EasyDirent* easydirent) {
    while (self->dirs_count > 0) {
        EasyDir easydir = self->dirs[self->dirs_count - 1];
        DIR* dir = easydir.dir;

        errno = 0;
        struct dirent* dirent = readdir(dir);
        if (dirent == NULL) {
            if (errno != 0) {
                fprintf(stderr, "Couldn't read dir: %s\n", strerror(errno));
                return false;
            }

            closedir(dir);
            self->dirs_count--;
            EASYDIR_FREE(easydir.path);

            return false;
        }

        int n = snprintf(easydirent->path, PATH_MAX, "%s/%s", easydir.path, dirent->d_name);
        ((char*)easydirent->path)[n] = 0;

        if (dirent->d_type == DT_DIR) {
            if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, "..")) continue;
            easydir_open(self, easydirent->path);
            continue;
        }

        easydirent->entry = dirent;
        return true;
    }

    return false;
}
#endif // EASYDIR_IMPLEMENTATION

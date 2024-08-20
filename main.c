#include <stdio.h>

#define EASYDIR_IMPLEMENTATION
#include "easy_dirent.h"

int main(void) {
    EasyDirHandle handle = {0};
    if (!easydir_open(&handle, ".")) return 1;

    EasyDirent dirent = {0};
    while (easydir_read(&handle, &dirent)){
        printf("%s\n", dirent.path);
    }

    return 0;
}

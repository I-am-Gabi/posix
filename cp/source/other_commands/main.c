//
// Created by Gabriela on 04/04/16.
//

#include <string.h>
#include <stdio.h>
#include "include/cp.h"

int main(int argc, char **argv) {
    if (find_directory(argv) == TRUE) {
        if (strcmp(argv[1], "-v") == 0)
            copy_file_directory(argv, TRUE);
        else
            copy_file_directory(argv, FALSE);

    } else if (argc <= 4) {
        if (strcmp(argv[1], "-v") == 0) {
            printf("%s -> %s\n", argv[2], argv[3]);
            copy_file(argv[2], argv[3]);
        } else {
            copy_file(argv[1], argv[2]);
        }
    } else {
        puts("Number of arguments invalid");
    }
}
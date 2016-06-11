//
// Created by Gabriela on 31/03/16.
//

#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#include <string.h>

#include "../include/lsrec.h"

char* directory;
char* arguments[] = {"-a", "-la", "-al", "-laR"};
char param[3];

int main(int argc, char **argv)
{
    // get the directory
    if (argc > 2) {
        strncpy(param, argv[1], 3);
        directory = argv[2];
    } else if (argc == 2) {
        strncpy(param, argv[1], 3);
        directory = ".";
    } else {
        strncpy(param, "-l", 3);
        directory = ".";
    }

    // search user database for a user ID
    struct passwd* pwd = getpwuid(geteuid());
    // group
    struct group *grp = getgrgid(geteuid());

    if (strcmp(param, arguments[0]) == 0 || (strcmp(param, arguments[1]) || strcmp(param, arguments[2]) == 0)) {
        lsrec_simple(pwd, grp, directory);
    } else if (strcmp(param, arguments[3])) {
        lsred_recursive(pwd, grp, directory);
    }

    return(0);
}


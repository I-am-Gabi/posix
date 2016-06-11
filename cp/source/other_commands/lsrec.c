//
// Created by Gabriela on 31/03/16.
//

#include <grp.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "../include/lsrec.h"

int lsred_recursive(struct passwd* pwd, struct group *grp, char* directory) {
    DIR * dir;

    // open the directory
    if ((dir = opendir(directory)) == NULL) {
        printf("%s: No such file or directory", directory);
    }

    // struct dirent
    struct dirent* dp_list;

    while ((dp_list = readdir(dir)) != NULL) {
        printf("dir %s \n", dp_list->d_name);
        if(dp_list->d_type == DT_DIR) {
            // strcat(directory, "/");
            // strcat(directory, dp_list->d_name);
            printf("---");
            lsrec_simple(pwd, grp, dp_list->d_name);
        }
        puts("");
    }

    return 0;
}

int lsrec_simple(struct passwd* pwd, struct group *grp, char* directory) {

    // structure to stock the open directory
    DIR * dir;

    // open the directory
    if ((dir = opendir(directory)) == NULL) {
        printf("%s: No such file or directory", directory);
    }

    // struct dirent
    struct dirent* dp_list;

    while ((dp_list = readdir(dir)) != NULL) {
        if (!strcmp (dp_list->d_name, "."))
            continue;
        if (!strcmp (dp_list->d_name, ".."))
            continue;

        struct stat fileStat;
        if (stat(dp_list->d_name, &fileStat) < 0)
            return 1;

        // print permissions
        print_permissions(&fileStat);
        print_link(&fileStat);
        print_owner_name(pwd);
        print_group_name(grp, &fileStat);
        print_file_size(&fileStat);
        print_data(&fileStat);
        print_name_file(dp_list);

        puts("");
    }

    closedir(dir);

    return 0;
}

void print_name_file(const struct dirent *dp_list) {// print file name
    printf("\t\t%s", dp_list->d_name);
}

void print_data(struct stat *fileStat) {// print data
    char date[10];
    strftime(date, 10, "%b %d", gmtime(&((*fileStat).st_ctime)));
    printf("\t%s", date);
}

void print_file_size(struct stat *fileStat) {// print file size
    printf("\t%9jd", (intmax_t)fileStat->st_size);
}

void print_group_name(struct group *grp, struct stat *fileStat) {/* Print out group name if it is found using getgrgid(). */
    if ((grp = getgrgid((*fileStat).st_gid)) != NULL)
            printf(" %-8.8s", grp->gr_name);
        else
            printf(" %-8d", (*fileStat).st_gid);
}

void print_owner_name(const struct passwd *pwd) {// print onwer name
    printf("\t%s", pwd->pw_name);
}

void print_link(struct stat *fileStat) {// print link
    printf("%4d", (*fileStat).st_nlink);
}

void print_permissions(struct stat *fileStat) {
    char permissions[] = {'r', 'w', 'x'};
    int constants[] = { S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
    printf((S_ISDIR((*fileStat).st_mode)) ? "d" : "-");
    for (int i = 0; i < 9; ++i)
        print(fileStat, constants[i], permissions[i % 3]);

    printf("\t");
}

void print(struct stat *fileStat, int constant, char permission) {
    if (((*fileStat).st_mode & constant))
        printf("%c", permission);
    else
        printf("-");
}
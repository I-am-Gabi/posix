#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <ecpglib.h>
#include <sys/stat.h> 
#include <dirent.h>
#include <stdlib.h>

#include "../include/cp.h"

/**
 *  TD - CP recursive
 *
 *  student: Gabriela Cavalcante da Silva
 *  version: 04/04/2016
 *
 *  Based in the program http://users.polytech.unice.fr/%7Ejpr/dokuwiki/doku.php?id=c_sys:c_sys
 *  _mkdir recursive based: http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
 *  permissions: http://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
 *
 *  TEST: The script to tests can be find in the root directory (cp-integration-test.sh)
 *
 *  Example:
 * - make a simple copy of a file to other
 *      $ ./cp exemple1.txt exemple2.txt
 * - make a simple copy with -v
 *      $ ./cp -v exemple1.txt exemple2.txt
 *      output example:
 *          exemple1.txt -> exemple2.txt
 *
 * - make a copy of a file set to a directory
 *      $ ./cp exemple1.txt exemple2.txt exemple3.txt exemple1_dir/
 * - make a copy of a file set to a directory with -v
 *      $ ./cp -v exemple1.txt exemple2.txt exemple3.txt exemple1_dir/
 *      output example:
 *          exemple1.txt -> exemple1_dir/exemple1.txt
 *          exemple2.txt -> exemple1_dir/exemple2.txt
 *          exemple3.txt -> exemple1_dir/exemple3.txt
 *
 *  TODO: to directories with many levels, was necessary make a mkdir recursive.
 */

//! Permission to read and write
#define USER_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) /* rw-rw-rw- */

//! Buffer size
#define BUF_SIZE 1024

//! buffer memory
char buffer[BUF_SIZE];

void create_dir(const char *path_destine);

void remove_slash(char *tmp, size_t len);

void check_flag_v(char *src, char* to, int flag_v);

//! main method to make the copy
void cp(char** argv, int flag_v, int flag_i, int flag_r) {
    int index = getIndex(flag_v, flag_i, flag_r);

    if (flag_r) {
        copy_recursive(argv[index], argv[index + 1], flag_v);
    } else if (find_directory(argv) == TRUE) {
        copy_file_directory(argv, flag_v, flag_i, flag_r);
    } else {
        if (flag_i && validation(argv[index]) == FALSE)
            return;

        check_flag_v(argv[index], argv[index + 1], flag_v);
        copy_file(argv[index], argv[index + 1]);
    }
}

void check_flag_v(char *src, char* to, int flag_v) {
    if (flag_v)
        printf("%s -> %s\n", src, to);
}

//! copy recursive
void copy_recursive(char *src, char *to, int flag_v) {
    DIR* dir;
    struct dirent* direntp;

    if (!(dir = opendir(src)))
        return;

    while ((direntp = readdir(dir))) {
        char* path_complete = malloc(strlen(src) + strlen(direntp->d_name) + 2);

        remove_slash(src, strlen(src));
        remove_slash(to, strlen(to));

        sprintf(path_complete, "%s/%s", src, direntp->d_name);

        if (direntp->d_type == DT_DIR) {
            if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
                continue;

            char* path_destine = malloc(strlen(to) + strlen(direntp->d_name) + 2);
            sprintf(path_destine, "%s/%s", to, direntp->d_name);

            copy_recursive(path_complete, path_destine, flag_v);
            free(path_destine);
        } else {
            char* path_destine = malloc(strlen(to) + strlen(direntp->d_name) + 2);
            sprintf(path_destine, "%s/", to);

            create_dir(path_destine);

            sprintf(path_destine, "%s%s", path_destine, direntp->d_name);

            check_flag_v(path_complete, path_destine, flag_v);
            copy_file(path_complete, path_destine);
            free(path_destine);
        }
        free(path_complete);
    }
    closedir(dir);
}

//! mkdir recursive
static void _mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    remove_slash(tmp, len);
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            // S_IRWXU = S_IRUSR | S_IWUSR | S_IXUSR
            // read, write and execute permission for the owner
            puts(tmp);
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    puts(tmp);
    mkdir(tmp, S_IRWXU);
}

//! remove the last slash
void remove_slash(char *tmp, size_t len) {
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
}

//! create directory
void create_dir(const char *path_destine) {
    //puts(path_destine);
    struct stat st = {0};
    if (stat(path_destine, &st) == -1) {
        _mkdir(path_destine);
    }
}


//! method to make a validation, if is possible continue
int validation(char* file) {
    printf("overwrite %s? (y/n [n]) ", file);
    int response = fgetc(stdin);
    if (response == 'y') {
        fgetc(stdin);
        return TRUE;
    }
    fgetc(stdin);
    return FALSE;
}

//! copy the file \a src in the file \a dest
void copy_file(const char *src, const char *to) {
    // file source
    int fdsrc;
    // file destine
    int fddest;
    int n;

    // open source file for reading only
    fdsrc = open(src, O_RDONLY);
    // check if there was some error
    if (fdsrc == -1)
        perror("error to open file source");

    // open for writing, create if nonexistent, truncate to zero length if exists
    fddest = open(to, O_WRONLY | O_CREAT | O_TRUNC, USER_PERM);
    // check if there was some error
    if (fddest == -1)
        perror("error to open file destine");

    // open the files | n = number of characters affected
    while ((n = read(fdsrc, buffer, BUF_SIZE)) > 0) {
        if (n == -1)
            perror("error to read the file source");
        if (write(fddest, buffer, n) != n)
            perror("error to write in the file of destine");
    }

    if (close(fdsrc) == -1 || close(fddest) == -1)
        perror("error to close the file(s)");
}

//! check if path is a directory
int is_directory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

//! return the initial index
int getIndex(int flag_v, int flag_i, int flag_r) {
    return 1 + ((flag_v == TRUE)?1:0) + ((flag_i == TRUE)?1:0) + ((flag_r == TRUE)?1:0);
}

//! make a copy of file to a directory
void copy_file_directory(char **args, int flag_v, int flag_i, int flag_r) {
    char* ptr_dir;
    int index = getIndex(flag_v, flag_i, flag_r);

    for (int i = index; i < sizeof(args); i++) {
        ptr_dir = args[i];
        if (is_directory(ptr_dir)) {
            for (int j = index; j < i; ++j) {
                char* ptr_file = args[j];
                char file_destine[255];
                sprintf(file_destine, "%s%s", ptr_dir, ptr_file);
                if (flag_i == TRUE) {
                    if (validation(file_destine) == FALSE)
                        continue;
                }
                if (flag_v == TRUE)
                    printf("%s -> %s\n", ptr_file, file_destine);
                copy_file(ptr_file, file_destine);
            }
            break;
        }
    }
}

//! check if there is a directory in the arguments
int find_directory(char **argv) {
    char* ptr_dir;
    for (int i = 1; i < sizeof(argv); i++) {
        ptr_dir = argv[i];
        if (is_directory(ptr_dir)) {
            return TRUE;
        }
    }
    return FALSE;
}
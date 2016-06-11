//
// Created by Gabriela on 04/04/16.
//

#ifndef MANIPULATION_DE_FICHIERS_CP_H
#define MANIPULATION_DE_FICHIERS_CP_H

#define FALSE 0
#define TRUE 1

void copy_file(const char *src, const char *to);

int is_directory(const char *path);

void copy_file_directory(char **args, int flag_v, int flag_i, int flag_r);

int find_directory(char **argv);

int validation(char* file);

int getIndex(int flag_v, int flag_i, int flag_r);

void cp(char** argv, int flag_v, int flag_i, int flag_r);

void copy_recursive(char *src, char *to, int flag_v);

#endif //MANIPULATION_DE_FICHIERS_CP_H

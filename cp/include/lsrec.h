//
// Created by Gabriela on 31/03/16.
//

#ifndef MANIPULATION_DE_FICHIERS_LSRED_H
#define MANIPULATION_DE_FICHIERS_LSRED_H


int lsred_recursive(struct passwd* pwd, struct group *grp, char* directory);

int lsrec_simple(struct passwd* pwd, struct group *grp, char* directory);

void print_name_file(const struct dirent *dp_list);

void print_data(struct stat *fileStat);

void print_file_size(struct stat *fileStat);

void print_group_name(struct group *grp, struct stat *fileStat);

void print_owner_name(const struct passwd *pwd);

void print_link(struct stat *fileStat);

void print_permissions(struct stat *fileStat);

void print(struct stat *fileStat, int constant, char permission);

#endif //MANIPULATION_DE_FICHIERS_LSRED_H

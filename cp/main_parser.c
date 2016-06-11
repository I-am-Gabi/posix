//
// Created by Gabriela on 04/04/16.
//

#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include "include/cp.h"

int flag_v;
int flag_i;
int flag_r;

void parser(int argc, char **argv) {
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "vir")) != -1) {
        switch (c) {
            case 'v':
                flag_v = 1;
                break;
            case 'i':
                flag_i = 1;
                break;
            case 'r':
                flag_r = 1;
                break;
            default:
                return;
        }
    }
}

int main(int argc, char **argv) {
    parser(argc, argv);

    cp(argv, flag_v, flag_i, flag_r);
}
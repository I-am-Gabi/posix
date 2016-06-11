//
// Created by Gabriela on 31/03/16.
//

#include <stdio.h>

char** env;

int main(int argc, char **argv, char** envp)
{
    for (env_variables = envp; *env_variables != 0; env_variables++)
    {
        char* this_env = *env_variables;
        printf("%s\n", this_env);
    }
    return(0);
}
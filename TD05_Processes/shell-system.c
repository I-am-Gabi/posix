/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 * $Id: shell-system.c,v 1.3 2005/04/12 15:49:36 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Simulation de la boucle principale du shell
 *
 * Cette version utilise une version (d'ailleurs incomplète) de system()
 * ------------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

/* Déclaration en avant */
int my_system(const char *);

/* Exécution directe des commandes built-in */
int exec_builtin(char *line);

/* Affichage du prompt */
void prompt();

/* Buffer pour la ligne de commande : LINE_MAX devrait être défini sous POSIX */
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

int main()
{
    char line[LINE_MAX];

    /* Boucle du shell */
    prompt();
    while (fgets(line, LINE_MAX, stdin) != NULL)
    {
        /* On enlève le \n et on vérifie que la ligne n'est pas vide
         *
         * En fait il faudrait faire des vérifications plus fines, par exemple
         * tester que la ligne lue contient bien une commande. Ceci est laissé
         * à titre d'exercice.
         */
        line[strlen(line) - 1] = '\0'; 
        if (line[0] == '\0') 
        {
            prompt();
            continue;
        }

        if (!exec_builtin(line)) my_system(line);
        prompt();
    }

    /* Fin du programme */
    exit(0);
}

/* La fonction system() elle-même
 *
 * ATTENTION : il s'agit d'une version *rustique* qui ne gère pas correctement
 * les signaux (voir le chapitre consacré aux signaux dans le cours)
 */
int my_system(const char *cmd)
{
    char *shell;
    int status;

    /* Récupère le shell de l'utilisateur : le shell par défaut est "sh" */   
    shell = getenv("SHELL");
    if (shell == 0 || strcmp(shell, "") == 0) shell = "sh";

    /* Exécution de la commande */
    switch (fork())
    {
    case -1: /* Erreur */
	perror("fork");
	abort();

    case 0: /* Processus fils qui exécute la commande */

	execlp(shell, shell, "-c", cmd, 0);
        perror("exit ls");	/* On ne doit pas passer ici */
	exit(1);         	/* Fin du fils : surtout pas return ! */

    default: /* Le père attend et récupère le code de retour */
	wait(&status); 
	break;	   
    }

    /* On retourne le code renvoyé par le shell */
    return status;
}

/* Exécution directe des commandes built-in */

int exec_builtin(char *line)
{
    char *cmd;
    char line1[LINE_MAX];

    /* ATTENTION ! strtok() modifie la chaine ! Il vaut mieux faire une copie */
    strcpy(line1, line);

    /* Recherche du nom de la commande */
    cmd = strtok(line1, " ");
    
    /* ici, on n'a une seule commande built-in, cd */
    if (strcmp(cmd, "cd") == 0)
    {
        char *dir = strtok(NULL, " ");
        if (chdir(dir) < 0) perror("chdir");
        if (strtok(NULL, " ") != NULL)
            fprintf(stderr, "Commande cd : trop d'arguments (ignorés)\n");  

        return 1;
    }    

    /* Ici, ce n'était pas une commande builtin */
    return 0;
}

/* Affichage du prompt */
void prompt()
{
    printf("$ ");
    fflush(stdout);
}


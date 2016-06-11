/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_pipes.c,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Redirections avec des tubes
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>

/* Fermeture des deux descripteurs d'un pipe */
static void close_pipe(int p[2])
{
    close(p[0]);
    close(p[1]);
}

/* Programme principal
 * -------------------
 *
 * NOTE : il est indispensable que chaque processus ferme les descripteurs de
 * pipes qu'il n'utilise pas (cad, ici, tous, une fois les redirections
 * réalisées).
 *
 * En effet, les processus grep et wc se termine sur une fin de fichier (EOF)
 * en entrée. Pour que EOF soit détecter sur un pipe, il faut qu'il n'y ait
 * plus AUCUN processus qui ait ce pipe ouvert en écriture.
 *
 * L'expérience est simple à faire : commentez le corps de la fonction
 * close_pipe() ci-dessus, recompilez, et exécutez. Votre exécution va se
 * bloquer, et ps (dans une autre fenêtre vous montrera que vos processus gre
 * et wc sont toujours vivants.
 */
int main()
{
    int p1[2];
    int p2[2];
    int i;

    switch (fork()) 
    {
    case -1:
        perror("fork");
        exit(1);

    case 0: /* Le fils exécute l'ensemble du pipe */
        
        /* On crée les deux pipes */
        if (pipe(p1) < 0 || pipe(p2) < 0)
        {
            perror("pipes");
            exit(1);
        }

        /* On crée 3 fils successifs Pour alléger le code, on ne tient pas
        * compte des erreurs possibles lors du fork() (d'ailleurs assez
        * rares !)
        */

        /* Premier fils : il exécute ps */
        if (fork() == 0) 
        {
            dup2(p1[1], 1);	/* redirection de la sortie standard */
            close_pipe(p1);	/* ces descripteurs sont maintenant inutiles */
            close_pipe(p2);
            execlp("ps", "ps", "ax", NULL);
            perror("exit ps");
            exit(1);
        }

        /* Deuxième fils : il exécute grep */
        if (fork() == 0) 
        {
            dup2(p1[0], 0);	/* redirection de l'entrée standard */
            dup2(p2[1], 1);	/* redirection de la sortie standard */
            close_pipe(p1);	/* ces descripteurs sont maintenant inutiles */
            close_pipe(p2);
            execlp("grep", "grep", "emacs", NULL);
            perror("exit grep");
            exit(1);
        }

        /* Troisième fils : il exécute wc */
        if (fork() == 0) 
        {
            dup2(p2[0], 0);	/* redirection de l'entrée standard */
            close_pipe(p1);	/* ces descripteurs sont maintenant inutiles */
            close_pipe(p2);
            execlp("wc", "wc", "-l", NULL);
            perror("exit wc");
            exit(1);
        }       
     
        // default: /* Le père intermédiaire attend tous ses fils (3) */
        close_pipe(p1);	/* ces descripteurs sont maintenant inutiles */
        close_pipe(p2);
        for (i = 0; i < 3; ++i) 
            wait(0);
    }

    wait(0);
    return 0;
}
        

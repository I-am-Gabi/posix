/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_redirect.c,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Redirections élémentaires
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Permissions par défaut des fichiers créés : rw pour tous
 * (elles seront en plus filtrées par le umask)
 */
#define USER_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/* La commande à exécuter, sous forme d'un tableau de même type que argv  */
char *params[] = {
    "ls",		/* le nom de la commande */
    "ring.c",		/* un nom de fichier (existant dans ce répertoire */
    "station.c",	/* idem */
    "foo",		/* un nom de fichier (inexistant dans ce répertoire */
    "bar",		/* idem */
    NULL		/* indispensable */
};

int main()
{
    int fd;

    switch (fork()) 
    {
    case -1:
        perror("fork");
        exit(1);

    case 0: /* Le fils exécute la commande */
        
        /* Auparavant il faut efffectuer les redirections */
        fd = open("out", O_WRONLY | O_CREAT | O_TRUNC, USER_PERM);
        if (fd < 0)
        {
            perror("open");
            exit(1);
        }
        dup2(fd, 1); 	/* redirection de la sortie standard */
        close(fd);	/* fd ne sert plus à rien maintenant */
        dup2(1, 2);	/* efffectue le 2>&1 : attention au sens ! */
        execvp("ls", params);
        perror("execvp");
        exit(1);

    default: /* Le père attend */
 
        wait(0);
    }

    return 0;
}
        

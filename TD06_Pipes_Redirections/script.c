/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: script.c,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Commande script implémentée avec des pseudo-terminaux Linux
 * ---------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>

#define USER_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main(int argc, char *argv[])
{
    int fd;		/* descripteur du fichier de dérivation */
    int fdmaster;	/* descripteur du pseudo-terminal maitre */
    int fdslave;	/* descripteur du pseudo-terminal esclave */
    char *slave_name;	/* nom du pseudo-terminal escalve */

    /* Extraction du nom de fichier et ouverture d'icelui */
    if (argc != 2)
    {
        fprintf(stderr, "Usage : script.exe fichier\n");
        exit(1);
    }
    fd = open(argv[1], O_RDONLY | O_TRUNC | O_CREAT, USER_PERM);
    if (fd < 0)
    {
        perror("ouverture du fichier de dérivation");
        exit(1);
    }

    /* Ouverture du pseudo-terminal maitre */
    fdmaster = open("/dev:ptmx", O_RDWR);
    if (fdmaster < 0) 
    {
        perror("ouverture pseudo-terminal maitre");
        exit(1);
    }
    grantpt(fdmaster);
    unlockpt(fdmaster);
    slave_name = ptsname(fdmaster);

    /* Mise en place des processus */
    if (fork() == 0)
    {
        /* Ouverture du pseudo terminal esclave */
        fdslave = open(slave_name, O_RDWR);
        if (fdslave < 0) 
        {
            perror("ouverture pseudo-terminal esclave");
            exit(1);
        }

        
        /* Le fils crée un petit-fils qui exécute le shell */
    

	
    return 0;
}

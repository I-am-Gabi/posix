/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 * $Id: multiple_fork.c,v 1.2 2005/04/11 07:43:52 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Ce programme met en évidence larallèle de plusieurs fils du même processus père.
 * ------------------------------------------------------------------------------------
 */
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NPROCESS 10
#define NLOOP 10

int main()
{
    int i;
    int j;

    /* Création de 10 processus s'exécutant en parallèle */

    for (i = 0; i < NPROCESS; ++i)
    {
	switch (fork())
	{ 
	case -1:
	    perror("fork");
	    abort();

	case 0:
	    /* Processus fils de numéro i */

	    /* Chaque fils affiche 10 fois son numéro (i)  */
	    for (j=0; j < NLOOP; ++j)
            {
		printf("%1d\n", i);
                /* Avec la ligne suivante, les exécutions s'entrelacent */
                /* sleep(1); */
            }

	    /* Fin du fils : ATTENTION à ne pas l'oublier, sinon création d'un
	     * grand nombre (exponentiel) de processus */
	    exit(0);

	default:
	    /* Suite du processus père */

	    /* On reboucle pour créer les fils suivants */
	    break;	   
	}
    }

    /* Mesure de salubrité sociale, le père attend la fin de tous ses fils */
    for (i = 0; i < 10; ++i) wait(0);

    /* On flushe la sortie standard */
    fflush(stdout);

    /* Ce message va sur l'erreur standard pour éviter de polluer la sortie */
    fprintf(stderr, "Vérifier le nombre de caractères (en principe 200)"
            " en \"pipant\" avec \'wc -c\'\n");

    /* Fin du père */
    exit(0);
}



/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 * $Id: exec_prop.c,v 1.1 2005/03/31 14:31:31 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Propriétés du recouvrement par execXX()
 *
 * Ce programme met en évidence les points suivants :
 *
 *	- après un fork() les buffers d'E/S de stdio sont hérités, mais ils
 *	  sont écrasés après exec()
 *	- exec() transforme le profgramme en cours d'exécution en un autre programme 
 *	  (recouvrement) mais ne change pas de processus
 * ------------------------------------------------------------------------------------
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int main()
{
     /* Premier cas: après un fork(), les buffers d'E/S de stdio sont hérités
      * ---------------------------------------------------------------------
      */

    /* Le père imprime un message, sans flush :
       Le message reste donc dans le buffer, sans être écrit sur stdout
     */
    printf("PID du père : %d \n", getpid()); /* noter absence de \n */

    switch (fork())
    {
    case -1: /* Erreur */
        perror("fork");
        abort();

    case 0: /* Processus fils */
        /* Affichage d'un message avec flush :
         *  on doit hériter (d'une copie) du buffer du père et donc imprimer
         *  une première fois "PID du père ..." avant
         */
        printf("PID du fils = %d\n", getpid()); /* Ici on met \n */

        /* Fin du fils */
        exit(0);

    default: /* Suite du processus père */
        /* Cette impression a pour effet secondaire de flusher le buffer du père :
         * on doit donc imprimer une seconde fois "PID du père ... " avant
         */
        printf("Continuation du père\n");
        break;

    }


    /* Deuxième cas: après un exec(), les buffers d'E/S de stdio sont écrasés
     * ----------------------------------------------------------------------
     */ 

    /* On se place dans la même situation: le père imprime un message, sans flush */
    printf("PID du père à nouveau :  = %d ", getpid()); 
				/* noter absence de \n : ce message va être *perdu* !*/

    /* Le père se transforme */
    execl("./exec_prop-aux.exe", "exec_prop-aux", NULL);
    perror("exec");	/* On ne devrait pas passer par ici */
    exit(0);		/* Fin du père en cas d'erreur dans execl() */
}



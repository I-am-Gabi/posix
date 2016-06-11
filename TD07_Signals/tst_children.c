/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Signaux
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_children.c,v 1.2 2005/04/12 17:51:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Transmission de signaux entre enfants
 * ---------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>

/* Pids des fils */
int eldest_pid;		/* ainé */
int youngest_pid;	/* cadet */

/* Piège pour SIGUSR1 */
void on_sigusr1(int sig)
{
    assert(sig == SIGUSR1);

    printf("Fils cadet reçoit SIGUSR1, envoit SIGUSR2 à son ainé et se termine\n");

    /* Le cadet connait le pid de son ainé, car il hérite de l'espace
     * d'adressage de son père où ce pid a été initialisé (la réciproque n'est
     * pas vrai, l'ainé ne peut connaitre son cadet).
     */
    kill(eldest_pid, SIGUSR2);  
    exit(0); 
}

/* Piège pour SIGUSR2 */
void on_sigusr2(int sig)
{
    assert(sig == SIGUSR2);

    printf("Fils ainé reçoit SIGUSR2 et se termine\n");
    exit(0);
}

int main(){

    struct sigaction sigact1;
    struct sigaction sigact2;
    sigset_t sigusr1_mask;
    sigset_t sigusr2_mask;

    /* On vide les masques: il n'y a rien de spécial à masquer dans le handler */
    sigemptyset(&sigusr1_mask);
    sigemptyset(&sigusr2_mask);

    switch (eldest_pid = fork())
    {
    case -1:
	perror("fork");
	exit(1);
      
    case 0:   
        /* Fils ainé */
        printf("Fils ainé : %d\n", getpid());

        /* Mise en place du piège de SIGUSR2 */
	sigact2.sa_handler = on_sigusr2;
	sigact2.sa_mask = sigusr2_mask;
	sigaction(SIGUSR2, &sigact2, NULL);
	printf("Fils ainé en attente\n");    
	sleep(5);
	exit(0);
      
    default:
        /* Le processus père continue, et crée le cadet */

	switch (youngest_pid = fork())
        {
	case -1:
	    perror("fork");
	    exit(1);
	  
	case 0:	 
            /* Fils cadet */
            printf("Fils cadet : %d\n", getpid());

            /* Mise en place du piège de SIGUSR1 */
	    sigact1.sa_handler = on_sigusr1;
	    sigact1.sa_mask = sigusr1_mask;
	    sigaction(SIGUSR1, &sigact1, NULL);
	    printf("Fils cadet en attente\n");    
	    sleep(5);
	    exit(0);
            break;
	}

        break;
        
    }

    /* Fin du père */

    /* Il faut laisser le temps au fils cadet de se préparer à "trapper" le signal */
    sleep(1); 

    printf("Père envoie SIGUSR1 au fils cadet\n");    
    kill(youngest_pid, SIGUSR1);

    /* Le père attend la fin de ses deux fils */
    wait(0);
    wait(0);	
    return 0;
}

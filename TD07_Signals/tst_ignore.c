/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Signaux
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_ignore.c,v 1.2 2005/04/12 17:51:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Diverses manières d'ignorer un signal
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

/* Temps d'attente */
int wait_time = 5;

/* Fonctions de capture des signaux (handler) 
 * -----------------------------------------
 */ 

/* Capture informative */
void on_sig(int sig)
{  

    if (sig == SIGINT) 
    {
        printf("Signal capturé : SIGINT\n"
               "\t(si ce signal n'avait pas été capturé, le processus serait mort)\n");
    }
    else
        printf("Signal capturé : %d\n", sig);
}

/* Capture sans rien faire */
void null_handler(int sig)
{}

/* Fonction mettant le processus en attente 
 * ----------------------------------------
 */ 
void process_wait()
{
    printf("Début d'attente de %d secondes : entrez des '^C' (SIGINT)\n"
           "\t(pour terminer ce programme, entrez '^\\')\n", wait_time);

    sleep(wait_time);

    printf("Fin d'attente\n"); 
}

/* Programme principal
 * -------------------
 */
int main()
{
    struct sigaction sigact;
    sigset_t sigmask;
    sigset_t sigint_mask;

    /* Préparation commune de sigact */
    sigemptyset(&sigmask);
    sigact.sa_mask = sigmask;

    /* Test 1 : on ignore SIGINT */   
    sigact.sa_handler = SIG_IGN;
    sigaction(SIGINT,  &sigact, NULL);
    process_wait();
    putchar('\n');

    /* Test 2 : on trappe SIGINT et on le masque, puis on attend, puis on démasque */

    sigact.sa_handler = on_sig;
    sigaction(SIGINT,  &sigact, NULL);
    sigemptyset(&sigint_mask);
    sigaddset(&sigint_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &sigint_mask, NULL);
    process_wait();
    /* On démasque SIGINT */
    sigprocmask(SIG_UNBLOCK, &sigint_mask, NULL);
    putchar('\n');

    /* Test 3 : on capture SIGINT avec une action vide */
    sigact.sa_handler = null_handler;
    sigaction(SIGINT,  &sigact, NULL);
    process_wait();
    putchar('\n');

    return 0;
}

    

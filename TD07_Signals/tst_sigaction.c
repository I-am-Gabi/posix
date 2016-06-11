/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Signaux
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_sigaction.c,v 1.2 2005/04/12 17:51:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Capture de signaux avec sigaction()
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>

/* Fonction de capture des signaux (handler) 
 * -----------------------------------------
 */ 
void on_sig(int sig)
{
    static int count = 0;
  
    switch (sig) 
    {
    case SIGSEGV:
        printf("Signal capturé : SIGSEGV\n");
        /* Que faire d'autre que terminer ? */
        exit(1);

    case SIGINT:  
        printf("Signal capturé : SIGINT\n");
        if (++count == 5) {
            printf("Reçu SIGINT 5 fois\n");
            exit(1);
        }
        break;

    default:
        /* Impossible de passer ici */
        assert(0);
        break;
    }
}  

/* Programme principal
 * -------------------
 * Il commence par attendre SIGINT ou un autre caractère, puis provoque un 
 * SIGSEGV en déréférençant le pointeur nul.
 *
 * Si ce programme est lancé en background, il est immunisé contre les signaux
 * émis par des caractères spéciaux au terminal (mais pas contre ceux
 * provenant de kill explicites (faits par exemple dans une autre fenêtre
 * xterm).
 */
int main()
{
    struct sigaction sigact;
    sigset_t sigmask;

    /* Mise en place du piège à signaux */
    sigemptyset(&sigmask);
    sigact.sa_handler = on_sig;
    sigact.sa_mask = sigmask;
    sigaction(SIGINT,  &sigact, NULL);
    sigaction(SIGSEGV,  &sigact, NULL);
		    
    /* On met le programme en attente : attention, sleep() ne conviendrait pas
     * ici (essayez, vous verrez et lisez la page du manuel de sleep (man 3
     * sleep).
     */
    printf("Faites\n"
           "\t- 5 SIGINT pour arrêter le programme\n"
           "\t\tsoit '^C' au terminal, si lancement en avant-plan\n"
           "\t\tsoit 'kill -INT %d', si lancement en arrière-plan\n" 
           "\t- envoyez des SIGSEGV (kill -SEGV %d)\n", getpid(), getpid());
    while (1) {}
  
    /* Fin du programme (jamais atteinte en fait) */
    return 0;
}

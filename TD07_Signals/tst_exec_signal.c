/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Signaux
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_exec_signal.c,v 1.2 2005/04/12 17:51:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Capture de signaux et execXX()
 * ---------------------------------------------------------------------------------
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/* Une fonction qui crée un fils exécutant le programme tst_exec_signal-aux.exe
 * et lui envoit SIGINT 
 */
void Exec()
{
    pid_t pid;

    if ((pid = fork()) != 0)
    {
        /* Père */

        /* On attent que le fils démarre */
        sleep(1); 

        kill(pid, SIGINT);
        wait(0);
    }
    else 
    {
        /* Fils */

        execl("./tst_exec_signal-aux.exe", "tst_exec_signal-aux", NULL);
        perror("exec");
        exit(0);
    }
}

void on_int(int sig)
{
    printf("signal %d\n", sig);
}

int main()
{
    printf("Le père ignore SIGINT: le fils doit aller jusqu'au bout (5 secondes)\n");
    signal(SIGINT, SIG_IGN);
    Exec();

    printf("Le père trappe SIGINT: le fils meure (signal rétabli à l'action par défaut)\n");
    signal(SIGINT, on_int);
    Exec();

    printf("Le père remet SIGINT à l'action par défaut : le fils meure immédiatement\n");
    signal(SIGINT, SIG_DFL);
    Exec();

    return 0;    
}

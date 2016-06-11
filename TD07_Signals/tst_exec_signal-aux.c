/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Signaux
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: tst_exec_signal-aux.c,v 1.2 2005/04/12 17:51:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Capture de signaux et execXX() : programme à exécuter
 * ---------------------------------------------------------------------------------
 */
#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("Début de tst_exec_signal-aux\n");
    sleep(5);
    printf("Fin de  tst_exec_signal-aux\n");

    return 0;
}

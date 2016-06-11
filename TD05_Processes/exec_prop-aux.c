/* ------------------------------------------------------------------------------------
 * Primitives de POSIX.1
 * ------------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * d'après une correction d'Erick Gallesio (ESSI)
 * $Id: exec_prop-aux.c,v 1.1 2005/03/31 14:31:31 jpr Exp $
 * ------------------------------------------------------------------------------------
 * Propriétés du recouvrement par execXX()
 *
 * Ce programme est utilisé (en fait *exécuté*) par exec_prop.exe
 * ------------------------------------------------------------------------------------
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    /* Affichage d'un message */
    printf("PID du processus après exec() : %d\n", getpid());

    /* Fin du processus */
    exit(0);
}

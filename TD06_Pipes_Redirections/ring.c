/* ---------------------------------------------------------------------------------
 * Primitives de POSIX.1 : Pipes et redirections
 * ---------------------------------------------------------------------------------
 * Jean-Paul Rigault --- ESSI --- Mars 2005
 * $Id: ring.c,v 1.1 2005/04/14 14:00:59 jpr Exp $
 * ---------------------------------------------------------------------------------
 * Réseau en anneau à jeton (token ring) : mise en place de l'anneau
 * ---------------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Définition globales
 * -------------------
 */

#define STATION_EXE "./station.exe"	/* nom de l'éxécutable d'une station */

#define MAX_STATIONS 10			/* Nombre maximum de stations */

int n_stations;				/* Nombre réel de stations */
int fd_pipes[MAX_STATIONS][2];		/* Descripteurs de tous les pipes */

/* Déclaration en avant */
void station(int i);

/* Fermeture de tous les descripteurs de pipe */
static void close_pipes()
{
    for (int j = 0; j < n_stations; j++)
    {
	close(fd_pipes[j][0]);
	close(fd_pipes[j][1]);
    }
}

/* Programme principal
 * -------------------
 *
 * On crée les pipes, puis les n processus fils correspondant aux stations. La
 * fonction station(i), invoquée par chaque fils, établit les connexions et
 * exécute le programme station.exe.
 */
int main(int argc, char *argv[])  
{ 
    int i; 

    /* Extraction du nombre de stations (passé en argument) */
    if (argc <= 1)
    {
	fprintf(stderr, "Usage : ring.exe n_stations\n");
	exit(1);
    }

    n_stations = atoi(argv[1]);
    if (n_stations <= 2 || n_stations >= MAX_STATIONS)
    {
	fprintf(stderr, "Vous voulez %d stations ? Soyez raisonnable! \n"
                "\t(Nombre maximum possible de stations : %d)\n", 
		n_stations, MAX_STATIONS );
	exit(1);
    }

    /* Création de tous les pipes */
    for (i = 0; i < n_stations; i++)
    {
        if (pipe(fd_pipes[i]) < 0) perror("création des tubes");
    }

    /* Création des processus des stations
     * -----------------------------------
     * Le processus Si lit sur le pipe i et écrit sur le pipe i+1. La structure
     * est circulaire: les calculs d'indice ont donc lieu modulo n_stations
     * (c'est pourquoi il vaut mieux éviter les valeurs négatives de i)
     */
    for (i = 0; i < n_stations; ++i)
    {
	switch(fork())
	{
	case -1:
	    perror("fork");
	    break;
	case 0:
	    /* Le fils appelle la mise en place de la station i */
	    station(i);
            /*NOTREACHED*/
	    break;
	default:
	    /* Le père passe à l'itèration suivante */
	    break;
	}
    }

    /* On peut maintenant fermer tous les descripteurs de pipes */
    close_pipes();

    /* Le père se met en attente de tous ses fils */
    for (i = 0; i < n_stations; i++) wait(NULL);

    /* Fin du programme */
    exit(0);
}

/* Mise en place d'une station
 * ---------------------------
 */
void station(int i)
{
    char ctab[5]; 
    char *arg[] = {STATION_EXE, NULL, NULL}; /* Argument de execvp */

    /* On redirige entree et sortie standard */
    dup2(fd_pipes[i][0], 0);
    dup2(fd_pipes[(i + 1) % n_stations][1], 1);

    /* On peut maintenant fermer tous les descripteurs de pipes */
    close_pipes();

    /* On exécute la station en passant i en argument */
    int nw = snprintf(ctab, 5, "%d", i); /* encodage de i sous forme d'une chaine */ 
    assert(nw < 5); /* voir manuel de snprintf() */
    arg[1] = ctab;
    execvp(STATION_EXE, arg);
    perror("exécution de la station");
    exit(1); /* surtout pas return ! */
}

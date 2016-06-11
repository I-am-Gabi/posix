/*!
 * \file main_mymalloc1.c 
 * \brief Un test de l'allocateur de mémoire simple.
 * 
 * \author Jean-Paul Rigault
 *
 * $Id: main_mymalloc1.c,v 1.2 2008/02/29 15:03:35 jpr Exp $
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "mymalloc.h"

#define N 100		//!< Nombre d'allocations par défaut.
#define SZMAX 10000	//!< Taille maximum à allouer par défaut.

/*!
 * \brief Premier test de l'allocateur de mémoire.
 *
 * On effectue nloop allocations de taille tirée au hasard (mais inférieure à
 * \a szmax), puis on désalloue dans l'ordre inverse.
 */

int main(int argc, char *argv[])
{
    int i; 		// indice de boucle d'allocation 
    int sz; 		// taille à allouer (en caractères) 
    void *tab[N];	// récupération des adresses de zone allouées 
    int nloop = N;	// nombre d'allocations 
    int szmax = SZMAX;	// taille maximale d'allocation 
    bool verbose = false; // contrôle l'affichage des messages

    // Arguments de la ligne de commande 
    int nopt = 0;
    if (argc > 1 && strcmp("-v", argv[1]) == 0)
    {
        verbose = true;
        ++nopt;
        --argc;
    }
    switch (argc)
    {
    case 1:
        break;
    case 2:
        nloop = atoi(argv[nopt + 1]);
        if (nloop > N) 
            nloop = N;
        break;
    case 3:
        nloop = atoi(argv[nopt + 1]);
        if (nloop > N) 
            nloop = N;
        szmax = atoi(argv[nopt + 2]);
        break;
    default:
        fprintf(stderr,"Usage: %s [-v] [nloop [szmax]]\n", argv[0]);
        exit(0);
    }

    printf("Starting mymalloc1 test: nloop = %d szmax = %d\n", nloop, szmax);

    mymalloc_instrum();   
  
    // Boucle d'allocation 
    for (i = 0; i < nloop;  ++i)
    {
        sz = rand() % szmax;  // on tire au sort la taille (entre 0 et szmax-1) 
        printf("Trying to Allocate size %d at tab[%d]\n", sz, i);
        tab[i] = mymalloc(sz);
        if (tab[i] != NULL)
        {
            if (verbose)
            {
                printf("Allocated size %d at tab[%d] = %p\n", sz, i, tab[i]);
                mymalloc_instrum();
            }
        }
        else 
            printf("Failed to allocate size %d at tab[%d]\n", sz, i);
    }

    // Boucle de déallocation, en sens inverse 
    for (i = nloop - 1; i >= 0; --i)
    {
        printf("Trying to deallocate tab[%d]\n", i);
        if (tab[i] != NULL) 
            myfree(tab[i]);
        if (verbose)
        {
            printf("Dellocated tab[%d] = %p\n", i, tab[i]);
            mymalloc_instrum();
        }
    }

    printf("End of mymalloc1 test\n");
    mymalloc_instrum();
    return 0;
  
}

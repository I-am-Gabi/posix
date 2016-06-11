/* ------------------------------------------------------------------
 * Un exemple d'implémentation de malloc()/free()
 * Jean-Paul Rigault --- ESSI --- Janvier 2005
 * $Id: main_mymalloc2.c,v 1.2 2008/02/29 15:03:35 jpr Exp $
 * ------------------------------------------------------------------
 * Programme de test 2
 * ------------------------------------------------------------------
 */
/*!
 * \file main_mymalloc2.c 
 * \brief Un test de l'allocateur de mémoire simple.
 * 
 * \author Jean-Paul Rigault
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "mymalloc.h"

#define N 1000		//!< Nombre d'allocations par défaut et nombre maximal.
#define SZMAX 10000	//!< Taille maximum à allouer par défaut.

/*!
 * \brief Deuxièeme test de l'allocateur de mémoire.
 *
 * On effectue \a nloop itérations. A chaque itération on tire à pile ou face pour
 * savoir si l'on fait une allocation ou une désallocation.
 *
 * Ce test tente donc de mélanger aléatoirement allocations et déallocations.
 */
int main(int argc, char *argv[])
{
    int i; 		// indice de boucle 
    int n;		// nombre d'allocations 
    int sz; 		// taille à allouer (en caractères) 
    void *tab[N];	// récupération des adresses de zone allouées 
    int nloop = N;	// nombre d'itérations 
    int szmax = SZMAX;	// taille maximale d'allocation 
    int nalloc = 0;	// nombre total d'allocations 
    int ndealloc = 0;	// nombre total de désallocations 
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

    printf("Starting mymalloc2 test: nloop = %d szmax = %d\n", nloop, szmax);
    mymalloc_instrum();
   
    // Boucle d'allocation 
    for (i = 0, n = 0; i < nloop;  ++i)
    {
        // Ce n'est pas vraiment pile ou face ! en moyenne 2 fois moins de
        // désallocations que d'allocations 
         
        if (rand() % 3 == 0 && n > 0)
        {
            // On désalloue le dernier 
            if (tab[--n] != 0)
                myfree(tab[n]);
            if (verbose) 
            {
                printf("Dellocated tab[%d] = %p\n", n, tab[n]);
                mymalloc_instrum();
            }
            ++ndealloc;
        }
        else
        {
            // On alloue 
            sz = rand() % szmax;	// on tire au sort la taille (entre 0 et szmax-1) 
            tab[n] = mymalloc(sz);
            if (tab[n] != NULL)
            {
                 if (verbose)
                     printf("Allocated size %d at tab[%d] = %p\n", sz, n, tab[n]);
                ++nalloc;
                ++n;
            }
            else 
                printf("Failed to allocate size %d at tab[%d]\n", sz, n);
            if (verbose)
                mymalloc_instrum(); 
        }
    }

    // Boucle de déallocation de ce qui reste, en sens inverse 
    for (i = n - 1; i >= 0; --i)
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

    printf("End of mymalloc2 test\n");
    printf("nalloc = %d ndealloc = %d\n", nalloc, ndealloc);
    mymalloc_instrum();
    return 0;
  }
